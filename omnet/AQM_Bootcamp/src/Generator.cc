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

#include "Generator.h"

Define_Module(Generator);

void Generator::initialize()
{
    packetInterval = par("packetInterval");
    currentInterval = packetInterval;
    cMessage *timer = new cMessage("timer");

    scheduleAt(simTime() + currentInterval, timer);
}

void Generator::handleMessage(cMessage *msg)
{
    EV << "Generator woke up at time: " << simTime() << endl;
    cMessage *packet = new cMessage("Packet");
    send(packet, "out");
    if (simTime() < 20)
        currentInterval = 0.80;

    else if (simTime() < 40)
        currentInterval = 0.15;

    else if (simTime() < 60)
        currentInterval = 0.60;

    else if (simTime() < 80)
        currentInterval = 0.20;

    else
        currentInterval = 0.80;
    scheduleAt(simTime() + currentInterval, msg);
}
