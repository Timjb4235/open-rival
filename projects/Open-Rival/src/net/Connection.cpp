#include "pch.h"

#include "net/Connection.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include "net/packets/AnonymousPacket.h"
#include "utils/BufferUtils.h"

namespace Rival {

Connection::Connection(
        Socket socket, std::shared_ptr<PacketFactory> packetFactory, int remoteClientId, ConnectionListener* listener)
    : socket(std::move(socket))
    , packetFactory(packetFactory)
    , remoteClientId(remoteClientId)
    , listener(listener)
    , receiveThread(&Connection::receiveThreadLoop, this)
{
    sendBuffer.reserve(maxBufferSize);
    recvBuffer.reserve(maxBufferSize);
}

Connection::~Connection()
{
    close();
    receiveThread.join();
}

bool Connection::operator==(const Connection& other) const
{
    return socket == other.socket;
}

bool Connection::operator!=(const Connection& other) const
{
    return !(*this == other);
}

void Connection::close() noexcept
{
    socket.close();
}

bool Connection::isClosed() const
{
    return socket.isClosed();
}

void Connection::receiveThreadLoop()
{
    while (!socket.isClosed())
    {
        // First read the packet size
        recvBuffer.resize(Packet::sizeBytes);
        socket.receive(recvBuffer);

        // Extract the packet size from the buffer
        std::size_t offset = 0;
        int nextPacketSize = 0;
        BufferUtils::readFromBuffer(recvBuffer, offset, nextPacketSize);
        recvBuffer.clear();

        // Sanity-check the packet size
        if (nextPacketSize > maxBufferSize)
        {
            throw std::runtime_error("Unexpected packet size: " + std::to_string(nextPacketSize));
        }

        // Now read the packet itself
        recvBuffer.resize(nextPacketSize);
        socket.receive(recvBuffer);

        // If this Connection has no packet factory, just wrap the buffers in anonymous packets. These are used by the
        // relay server, which doesn't care about their contents.
        std::shared_ptr<const Packet> packet = packetFactory
                ? packetFactory->deserialize(recvBuffer)
                : std::make_shared<AnonymousPacket>(recvBuffer, remoteClientId);

        if (packet)
        {
            // Pass packets directly to the listener if present, otherwise queue them until requested
            if (listener)
            {
                listener->onPacketReceived(*this, packet);
            }
            else
            {
                std::scoped_lock lock(receivedPacketsMutex);
                receivedPackets.push_back(packet);
            }
        }
    }
}

void Connection::send(const Packet& packet)
{
    packet.serialize(sendBuffer);
    packet.finalize(sendBuffer);
    if (sendBuffer.empty())
    {
        throw std::runtime_error("Tried to send empty buffer");
    }
    socket.send(sendBuffer);
    sendBuffer.clear();
}

std::vector<std::shared_ptr<const Packet>> Connection::getReceivedPackets()
{
    std::vector<std::shared_ptr<const Packet>> packetsToReturn;

    {
        std::scoped_lock lock(receivedPacketsMutex);
        packetsToReturn = receivedPackets;
        receivedPackets.clear();
    }

    return packetsToReturn;
}

}  // namespace Rival
