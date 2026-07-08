//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Queue.h"

Define_Module(Queue);

void Queue::initialize()
{

        busy = false;
        currentPacket = nullptr;
        endServiceEvent = new cMessage("endService");
        maxQueueSize = 10;
        packetsReceived = 0;
        packetsDropped = 0;
        packetsForwarded = 0;
}

void Queue::handleMessage(cMessage *msg)
{
    if (msg == endServiceEvent)
        {
            EV << "Finished service for "
               << currentPacket->getName()
               << " at "
               << simTime()
               << endl;

            send(currentPacket, "out");
            packetsForwarded++;

            if (buffer.empty())
            {
                busy = false;
                currentPacket = nullptr;
            }
            else
            {
                currentPacket = buffer.front();
                buffer.pop();

                EV << "Starting service for "
                   << currentPacket->getName()
                   << " at "
                   << simTime()
                   << endl;

                scheduleAt(simTime() + 1, endServiceEvent);
            }
        }

        // New packet arrived
        else
        {
            packetsReceived++;
            if (!busy)
            {
                busy = true;
                currentPacket = msg;

                EV << "Starting service for "
                   << currentPacket->getName()
                   << " at "
                   << simTime()
                   << endl;

                scheduleAt(simTime() + 1, endServiceEvent);
            }
            else
            {
                if (buffer.size() >= maxQueueSize)
                {
                    EV << "Packet dropped. Queue full!" << endl;

                    delete msg;
                    packetsDropped++;
                }
                else
                {
                    buffer.push(msg);

                    EV << msg->getName()
                    << " queued. Queue length = "
                    << buffer.size()
                    << endl;
                }


            }
}
}
void Queue::finish()
{
    EV << endl;
    EV << "----- Queue Statistics -----" << endl;
    EV << "Packets Received : " << packetsReceived << endl;
    EV << "Packets Forwarded: " << packetsForwarded << endl;
    EV << "Packets Dropped  : " << packetsDropped << endl;
    EV << "Final Queue Size : " << buffer.size() << endl;
    EV << "----------------------------" << endl;

    double pdr = (double)packetsForwarded / packetsReceived;
    double dropRate = (double)packetsDropped / packetsReceived;
    double throughput = (double)packetsForwarded / simTime().dbl();

    EV << "Packet Delivery Ratio : " << pdr << endl;
    EV << "Drop Rate             : " << dropRate << endl;
    EV << "Throughput            : " << throughput << " packets/sec" << endl;
}

