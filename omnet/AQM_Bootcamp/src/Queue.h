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

#ifndef __AQM_BOOTCAMP_QUEUE_H_
#define __AQM_BOOTCAMP_QUEUE_H_

#include <omnetpp.h>
#include <queue>
#include <fstream>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Queue : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    double calculateDropProbability();
    void logMetrics(double dropProbability, double randomNumber, bool packetDropped);
  private:
    std::queue<cMessage*> buffer;
    std::ofstream csvFile;
    cMessage *endServiceEvent;
    cMessage *currentPacket;
    bool busy;
    int maxQueueSize;
    int minThreshold;
    int maxThreshold;
    int packetsReceived;
    int packetsDropped;
    int packetsForwarded;


};

#endif
