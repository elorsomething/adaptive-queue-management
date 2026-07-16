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
        maxQueueSize = par("maxQueueSize");
        minThreshold = par("minThreshold");
        maxThreshold = par("maxThreshold");
        packetsReceived = 0;
        packetsDropped = 0;
        packetsForwarded = 0;
        csvFile.open("results/queue_metrics.csv");
        if(!csvFile.is_open())
            throw cRuntimeError("Cannot open CSV file.");

        csvFile << "Time,"
                << "QueueSize,"
                << "QueueOccupancy,"
                << "DropProbability,"
                << "RandomNumber,"
                << "PacketDropped,"
                << "PacketsDropped,"
                << "PacketsForwarded,"
                << "PacketsReceived,"
                << "ArrivalRate,"
                << "ServiceRate"
                << endl;
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
                logMetrics(calculateDropProbability(), -1 , false);
            }
            else
            {
                currentPacket = buffer.front();
                buffer.pop();
                logMetrics(calculateDropProbability(), -1, false);

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
            double r =-1;
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
                logMetrics(calculateDropProbability(), -1, false);
            }
            else
            {
                if (buffer.size() >= maxQueueSize)
                {
                    EV << "Packet dropped. Queue full!" << endl;
                    packetsDropped++;
                    delete msg;

                    logMetrics(calculateDropProbability(), -1, true);
                }
                else
                {
                if (buffer.size() >= minThreshold)
                {

                    double dropProbability = calculateDropProbability();

                    r = uniform(0.0,1.0);

                    EV << "Queue = "
                       << buffer.size()
                       << "  Drop Probability = "
                       << dropProbability
                       << "Random = "
                       << r
                       << endl;

                    if (r < dropProbability)
                    {
                        EV << "Early drop! Random = "
                           << r << endl;
                        packetsDropped++;
                        logMetrics(dropProbability, r, true);
                        delete msg;
                        return;
                    }
                }





                buffer.push(msg);
                logMetrics(calculateDropProbability(), r, false);

                    EV << msg->getName()
                    << " queued. Queue length = "
                    << buffer.size()
                    << endl;
                }


            }
        }
}
double Queue::calculateDropProbability()
{
    if (buffer.size() <= minThreshold)
        return 0.0;

    if (buffer.size() >= maxThreshold)
        return 1.0;

    return (double)(buffer.size() - minThreshold) /
           (maxThreshold - minThreshold);
}
void Queue::logMetrics(double dropProbability, double randomNumber, bool packetDropped)
{
    double occupancy = (double)buffer.size() / maxQueueSize;

    if (simTime() > 0)
    {
        arrivalRate = (double)packetsReceived / simTime().dbl();
        serviceRate = (double)packetsForwarded / simTime().dbl();
    }
    else
    {
        arrivalRate = 0;
        serviceRate = 0;
    }
    csvFile
        << simTime() << ","
        << buffer.size() << ","
        << occupancy << ","
        << dropProbability << ","
        << randomNumber << ","
        << packetDropped << ","
        << packetsDropped << ","
        << packetsForwarded << ","
        << packetsReceived << ","
        << arrivalRate <<","
        << serviceRate
        << endl;
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
    csvFile.close();
}

