#include "pch.h"

#include "GameState.h"

#include <map>
#include <stdexcept>
#include <utility>  // std::move

#include "net/Connection.h"
#include "Application.h"
#include "ApplicationContext.h"
#include "GameInterface.h"
#include "Image.h"
#include "InputUtils.h"
#include "MouseUtils.h"
#include "Palette.h"
#include "Race.h"
#include "RenderUtils.h"
#include "Spritesheet.h"
#include "TimeUtils.h"

namespace Rival {

GameState::GameState(
        Application& app, std::unique_ptr<World> scenarioToMove, std::unordered_map<int, PlayerState>& playerStates)
    : State(app)
    , world(std::move(scenarioToMove))
    , playerStates(playerStates)
    , viewport(0, 0, window->getWidth(), window->getHeight() - GameInterface::uiHeight)
    , camera(0.0f,
             0.0f,
             RenderUtils::pxToCamera_X(static_cast<float>(viewport.width)),
             RenderUtils::pxToCamera_Y(static_cast<float>(viewport.height)),
             *world)
    , mousePicker(camera, viewport, *world, playerContext, *this, *this)
    , gameRenderer(window, *world, *this, camera, viewport, res, playerContext)
{
}

void GameState::onLoad()
{
    app.getContext().getAudioSystem().playMidi(res.getMidi(0));
}

void GameState::update()
{
    pollNetwork();
    world->addPendingEntities();
    earlyUpdateEntities();
    respondToInput();
    updateEntities();
    processCommands();
    ++currentTick;
}

void GameState::pollNetwork()
{
    std::optional<Connection>& connection = app.getConnection();
    if (!connection || connection->isClosed())
    {
        return;
    }

    /*
    const auto& receivedPackets = connection->getReceivedPackets();
    for (const auto& packet : receivedPackets)
    {
        // TODO: Process packet
        //  - extract commands
        //  - schedule commands as needed
    }
    */
}

void GameState::earlyUpdateEntities() const
{
    auto const& entities = world->getMutableEntities();
    for (auto const& e : entities)
    {
        e->earlyUpdate();
    }
}

void GameState::respondToInput()
{
    mousePicker.handleMouse();

    // Move camera
    if (input.lastDirectionX == Direction::Decreasing)
    {
        camera.translate(-0.5f, 0.f);
    }
    else if (input.lastDirectionX == Direction::Increasing)
    {
        camera.translate(0.5f, 0.f);
    }
    if (input.lastDirectionY == Direction::Decreasing)
    {
        camera.translate(0.f, -0.35f);
    }
    else if (input.lastDirectionY == Direction::Increasing)
    {
        camera.translate(0.f, 0.35f);
    }
}

void GameState::updateEntities() const
{
    std::vector<std::shared_ptr<Entity>> deletedEntities;

    auto const& entities = world->getMutableEntities();
    for (auto const& e : entities)
    {
        if (e->isDeleted())
        {
            deletedEntities.push_back(e);
        }
        else
        {
            e->update();
        }
    }

    // Remove deleted Entities
    for (auto const& e : deletedEntities)
    {
        world->removeEntity(e);
        e->onDelete();
    }
}

void GameState::render(int delta)
{
    gameRenderer.render(delta);
}

void GameState::keyDown(const SDL_Keycode keyCode)
{
    switch (keyCode)
    {
    case SDLK_UP:
        input.lastDirectionY = Direction::Decreasing;
        break;

    case SDLK_DOWN:
        input.lastDirectionY = Direction::Increasing;
        break;

    case SDLK_LEFT:
        input.lastDirectionX = Direction::Decreasing;
        break;

    case SDLK_RIGHT:
        input.lastDirectionX = Direction::Increasing;
        break;

    default:
        break;
    }
}

void GameState::keyUp(const SDL_Keycode keyCode)
{
    switch (keyCode)
    {
    case SDLK_UP:
        input.lastDirectionY = InputUtils::isKeyDown(SDLK_DOWN) ? Direction::Decreasing : Direction::None;
        break;

    case SDLK_DOWN:
        input.lastDirectionY = InputUtils::isKeyDown(SDLK_UP) ? Direction::Increasing : Direction::None;
        break;

    case SDLK_LEFT:
        input.lastDirectionX = InputUtils::isKeyDown(SDLK_RIGHT) ? Direction::Decreasing : Direction::None;
        break;

    case SDLK_RIGHT:
        input.lastDirectionX = InputUtils::isKeyDown(SDLK_LEFT) ? Direction::Increasing : Direction::None;
        break;

    default:
        break;
    }
}

void GameState::mouseDown(const SDL_MouseButtonEvent evt)
{
    mousePicker.mouseDown(evt.button);
}

void GameState::mouseUp(const SDL_MouseButtonEvent evt)
{
    mousePicker.mouseUp(evt.button);
}

void GameState::mouseWheelMoved(const SDL_MouseWheelEvent evt)
{

    // Get the mouse position relative to the window, in pixels
    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Abort if the mouse is outside the viewport
    if (!viewport.contains(mouseX, mouseY))
    {
        return;
    }

    // Check if any scrolling took place
    int scrollAmount = evt.y;
    if (scrollAmount == 0)
    {
        return;
    }

    // Determine the normalized scroll amount
    if (evt.direction == SDL_MOUSEWHEEL_FLIPPED)
    {
        scrollAmount *= -1;
    }

    // Zoom
    float zoomBefore = camera.getZoom();
    if (scrollAmount > 0)
    {
        camera.modZoom(Camera::zoomInterval);
    }
    else
    {
        camera.modZoom(-Camera::zoomInterval);
    }
    float zoomAfter = camera.getZoom();

    // If the zoom level hasn't changed, there is no need to translate
    // the camera
    if (zoomBefore == zoomAfter)
    {
        return;
    }

    // Calculate mouse position relative to the viewport, in the range 0-1
    float normalizedMouseX = MouseUtils::getNormalizedMouseInViewportX(mouseX, viewport);
    float normalizedMouseY = MouseUtils::getNormalizedMouseInViewportY(mouseY, viewport);

    // Calculate mouse position relative to the viewport centre, in the
    // range -1 to 1
    float relativeMouseX = (2 * normalizedMouseX) - 1;
    float relativeMouseY = (2 * normalizedMouseY) - 1;

    // Move the camera based on the cursor position.
    // We move towards the cursor when zooming in, and away from the cursor
    // when zooming out.
    if (scrollAmount > 0)
    {
        camera.translate(relativeMouseX, relativeMouseY);
    }
    else
    {
        camera.translate(-relativeMouseX, -relativeMouseY);
    }
}

World& GameState::getWorld()
{
    return *world;
}

PlayerState& GameState::getLocalPlayerState() const
{
    auto result = playerStates.find(localPlayerId);
    if (result == playerStates.end())
    {
        throw std::runtime_error("No local player state found!");
    }

    return result->second;
}

PlayerState* GameState::getPlayerState(int playerId) const
{
    auto result = playerStates.find(playerId);
    return result == playerStates.end() ? nullptr : &result->second;
}

bool GameState::isLocalPlayer(int playerId) const
{
    return playerId == localPlayerId;
}

void GameState::dispatchCommand(std::shared_ptr<GameCommand> command)
{
    if (!command)
    {
        std::cerr << "Tried to dispatch invalid command";
        return;
    }

    // In multiplayer, schedule commands for 'n' ticks in the future
    int relevantTick = isNetGame() ? currentTick + TimeUtils::netCommandDelay : currentTick;

    // Schedule our command
    auto findResult = pendingCommands.find(relevantTick);
    if (findResult == pendingCommands.end())
    {
        // No commands are scheduled for this tick yet
        pendingCommands.insert({ relevantTick, { command } });
    }
    else
    {
        // Add our command to the commands already scheduled for this tick
        std::vector<std::shared_ptr<GameCommand>>& commandsDue = findResult->second;
        commandsDue.push_back(command);
    }

    // Send command to the server
    // CommandPacket packet(command);
    // app.getConnection()->send(packet);
}

void GameState::processCommands()
{
    auto findResult = pendingCommands.find(currentTick);
    if (findResult == pendingCommands.end())
    {
        // No commands due
        return;
    }

    std::vector<std::shared_ptr<GameCommand>>& commandsDue = findResult->second;
    for (auto& cmd : commandsDue)
    {
        cmd->execute(*this);
    }

    pendingCommands.erase(currentTick);
}

bool GameState::isNetGame() const
{
    return app.getConnection().has_value();
}

}  // namespace Rival
