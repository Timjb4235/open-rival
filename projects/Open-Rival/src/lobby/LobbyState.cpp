#include "lobby/LobbyState.h"

#include <cstdlib>  // std::rand

#include "application/Application.h"
#include "application/ApplicationContext.h"
#include "entity/EntityFactory.h"
#include "game/GameState.h"
#include "game/PlayerState.h"
#include "game/World.h"
#include "gfx/renderable/TextRenderable.h"
#include "net/packet-handlers/AcceptPlayerPacketHandler.h"
#include "net/packet-handlers/KickPlayerPacketHandler.h"
#include "net/packet-handlers/LobbyWelcomePacketHandler.h"
#include "net/packet-handlers/RejectPlayerPacketHandler.h"
#include "net/packet-handlers/RequestJoinPacketHandler.h"
#include "net/packet-handlers/StartGamePacketHandler.h"
#include "net/packets/AcceptPlayerPacket.h"
#include "net/packets/KickPlayerPacket.h"
#include "net/packets/LobbyWelcomePacket.h"
#include "net/packets/RejectPlayerPacket.h"
#include "net/packets/RequestJoinPacket.h"
#include "net/packets/StartGamePacket.h"
#include "scenario/ScenarioBuilder.h"
#include "scenario/ScenarioReader.h"
#include "utils/ConfigUtils.h"
#include "utils/LogUtils.h"

namespace Rival {

Rect makeViewport(const Window* window)
{
    // Fill the screen
    return { 0, 0, window->getWidth(), window->getHeight() };
}

LobbyState::LobbyState(Application& app, std::string playerName, bool host)
    : State(app)
    , host(host)
    , localPlayerName(playerName)
    , menuRenderer(res, window, makeViewport(window))
    , textRenderer(window)
{
    // Register PacketHandlers
    packetHandlers.insert({ PacketType::RequestJoin, std::make_unique<RequestJoinPacketHandler>() });
    packetHandlers.insert({ PacketType::AcceptPlayer, std::make_unique<AcceptPlayerPacketHandler>() });
    packetHandlers.insert({ PacketType::RejectPlayer, std::make_unique<RejectPlayerPacketHandler>() });
    packetHandlers.insert({ PacketType::LobbyWelcome, std::make_unique<LobbyWelcomePacketHandler>() });
    packetHandlers.insert({ PacketType::KickPlayer, std::make_unique<KickPlayerPacketHandler>() });
    packetHandlers.insert({ PacketType::StartGame, std::make_unique<StartGamePacketHandler>() });
}

void LobbyState::onLoad()
{
    // Load the initial level
    // TODO: Clients should wait to receive the level name from the host
    ApplicationContext& context = app.getContext();
    const std::string levelName = ConfigUtils::get(context.getConfig(), "levelName", std::string());
    if (levelName.empty())
    {
        throw std::runtime_error("No level name found in config.json ");
    }
    loadLevel(levelName);

    if (isNetGame())
    {
        if (host)
        {
            // Add ourselves to the lobby.
            // The host should always have a client ID and player ID of 0.
            ClientInfo localClient(0, localPlayerName);
            onPlayerAccepted(joinRequestId, 0, localClient);
        }
        else
        {
            // We generate a random request ID, just in case 2 players try to join with the same name
            joinRequestId = std::rand();
            RequestJoinPacket joinPacket(joinRequestId, localPlayerName);
            app.getConnection()->send(joinPacket);
        }
    }
    else
    {
        // Just start the game immediately
        localPlayerId = 0;
        startGame();
    }
}

void LobbyState::update()
{
    pollNetwork();
}

void LobbyState::render(int delta)
{
    menuRenderer.render(delta);
    renderText();
}

void LobbyState::renderText()
{
    // TMP: Hardcoded hacky rendering until we have a proper menu system.
    // The TextRenderables should be long-lived objects - we should not be recreating them every frame.
    std::vector<TextRenderable> textRenderables;
    TextProperties nameProperties = { &res.getFontRegular() };
    glm::vec2 renderPos = { 100, 100 };
    const float rowHeight = 32;
    const float indent = 32;

    // Header
    {
        TextSpan textSpan = { "Connected Players:", TextRenderable::defaultColor };
        textRenderables.emplace_back(textSpan, nameProperties, renderPos.x, renderPos.y);
        renderPos.x += indent;
        renderPos.y += rowHeight;
    }

    // Local player (if hosting, should always come first)
    if (host)
    {
        std::string name = localPlayerName;
        TextSpan textSpan = { name, TextRenderable::defaultColor };
        textRenderables.emplace_back(textSpan, nameProperties, renderPos.x, renderPos.y);
        renderPos.y += rowHeight;
    }

    // Other players
    for (const auto& entry : clients)
    {
        std::string name = entry.second.getName();
        TextSpan textSpan = { name, TextRenderable::defaultColor };
        textRenderables.emplace_back(textSpan, nameProperties, renderPos.x, renderPos.y);
        renderPos.y += rowHeight;
    }

    // Local player (if not hosting, always comes last for now; later we will sort by player ID)
    if (!host)
    {
        std::string name = localPlayerName;
        TextSpan textSpan = { name, TextRenderable::defaultColor };
        textRenderables.emplace_back(textSpan, nameProperties, renderPos.x, renderPos.y);
        renderPos.y += rowHeight;
    }

    // Render the text!
    std::vector<const TextRenderable*> textRenderablePtrs;
    for (const auto& textRenderable : textRenderables)
    {
        textRenderablePtrs.push_back(&textRenderable);
    }
    textRenderer.render(textRenderablePtrs);
}

void LobbyState::keyUp(const SDL_Keycode keyCode)
{
    switch (keyCode)
    {
    case SDLK_SPACE:
    case SDLK_RETURN:
        // TMP: For now, just start the game when pressing Space or Enter
        if (host)
        {
            requestStartGame();
        }
        break;
    default:
        break;
    }
}

void LobbyState::pollNetwork()
{
    if (!isNetGame())
    {
        return;
    }

    auto& connection = app.getConnection();
    const auto& receivedPackets = connection->getReceivedPackets();
    for (int i = 0; i < receivedPackets.size(); ++i)
    {
        const auto& packet = receivedPackets[i];
        const auto iter = packetHandlers.find(packet->getType());
        if (iter == packetHandlers.cend())
        {
            LOG_WARN("Received unexpected packet of type {}", EnumUtils::toIntegral(packet->getType()));
            continue;
        }

        iter->second->onPacketReceived(packet, *this);

        if (isGameStarted)
        {
            // Any packets received after this point are intended for the next state
            std::vector<std::shared_ptr<const Packet>> packetsToReturn(
                    receivedPackets.cbegin() + i + 1, receivedPackets.cend());
            connection->returnPackets(packetsToReturn);
            break;
        }
    }
}

void LobbyState::onPlayerJoinRequest(int requestId, int clientId, const std::string& playerName)
{
    if (!host)
    {
        // Only the host has the authority to accept other players
        return;
    }

    int playerId = requestPlayerId();
    if (playerId >= 0)
    {
        AcceptPlayerPacket acceptPacket(requestId, playerName, playerId);
        app.getConnection()->send(acceptPacket);

        ClientInfo client = { playerId, playerName };
        onPlayerAccepted(requestId, clientId, client);
    }
    else
    {
        RejectPlayerPacket rejectPacket(requestId, playerName);
        app.getConnection()->send(rejectPacket);
        onPlayerRejected(requestId, playerName);
    }
}

void LobbyState::onPlayerAccepted(int requestId, int clientId, const ClientInfo& client)
{
    if (requestId == joinRequestId && client.getName() == localPlayerName)
    {
        // Our request to join was the one that got accepted, which means this is our player ID!
        localPlayerId = client.getPlayerId();
        return;
    }

    LOG_INFO("Player {} has joined", client.getName());

    if (host)
    {
        // Inform joining player about the current lobby state
        std::unordered_map<int, ClientInfo> clientsIncludingHost = clients;
        ClientInfo localClient(0, localPlayerName);
        clientsIncludingHost.insert({ 0, localClient });
        LobbyWelcomePacket welcomePacket(client.getPlayerId(), clientsIncludingHost);
        app.getConnection()->send(welcomePacket);
    }

    clients.insert({ clientId, client });
}

void LobbyState::onPlayerRejected(int requestId, const std::string& playerName)
{
    LOG_INFO("Player {} was not allowed to join", playerName);

    if (requestId == joinRequestId && playerName == localPlayerName)
    {
        // Our request to join was the one that got rejected
        // TODO: return to main menu
        throw std::runtime_error("Rejected by server");
    }
}

void LobbyState::onWelcomeReceived(int playerId, std::unordered_map<int, ClientInfo> newClients)
{
    if (playerId != localPlayerId)
    {
        // This was not intended for us
        return;
    }

    LOG_INFO("Received lobby state from host");

    clients = newClients;
}

void LobbyState::onPlayerKicked(int playerId)
{
    LOG_INFO("Player {} was kicked", std::to_string(playerId));

    if (playerId == localPlayerId)
    {
        // TODO: return to main menu
        throw std::runtime_error("Kicked by server");
    }
}

int LobbyState::requestPlayerId()
{
    // TODO: Find the first available player ID
    // This is a hack! We should consider the max players and and empty slots from disconnected clients.
    int playerId = nextPlayerId;
    ++nextPlayerId;
    return playerId;
}

void LobbyState::loadLevel(const std::string& filename)
{
    ScenarioReader reader(Resources::mapsDir + filename);
    scenarioData = reader.readScenario();
}

void LobbyState::requestStartGame()
{
    if (!host)
    {
        LOG_WARN("Non-host player tried to start the game");
        return;
    }

    StartGamePacket packet;
    app.getConnection()->send(packet);
    startGame();
}

void LobbyState::startGame()
{
    LOG_INFO("Starting game!");
    std::unique_ptr<State> game = createGameState();
    app.setState(std::move(game));
    isGameStarted = true;
}

std::unique_ptr<State> LobbyState::createGameState() const
{
    ApplicationContext& context = app.getContext();

    // Create the world
    ScenarioBuilder scenarioBuilder(scenarioData);
    EntityFactory entityFactory(context.getResources(), context.getAudioSystem());
    std::unique_ptr<World> world = scenarioBuilder.build(entityFactory);

    // Initialize players
    int numPlayers = PlayerStore::maxPlayers;  // TODO: Read this from the Scenario
    std::unordered_map<int, PlayerState> playerStates;
    for (int playerId = 0; playerId < numPlayers; ++playerId)
    {
        const PlayerProperties& playerProps = scenarioData.playerProperties.at(playerId);
        playerStates.emplace(std::piecewise_construct,
                std::forward_as_tuple(playerId),
                std::forward_as_tuple(scenarioBuilder.getRace(playerProps.race),
                        static_cast<int>(playerProps.startingGold),
                        static_cast<int>(playerProps.startingWood),
                        static_cast<int>(playerProps.startingFood)));
    }

    return std::make_unique<GameState>(app, std::move(world), playerStates, clients, localPlayerId);
}

bool LobbyState::isNetGame() const
{
    return app.getConnection().has_value();
}

}  // namespace Rival
