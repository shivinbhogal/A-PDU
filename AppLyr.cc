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

#include "AppLyr.h"
#include <queue>

Define_Module(AppLyr);

void AppLyr::initialize()
{
    // TODO - Generated method body


    counter = 0;
    nodeid = par("nodeid");

    if(nodeid == 1)
    {
        //scheduleAt(simTime()+timeout, timeoutEvent);  //schedule timeout message for source node
        cMessage *cmsg = new cMessage();
        scheduleAt(0.0,cmsg);

    }
}

void AppLyr::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    counter++;
    if(msg->isSelfMessage())
    {
        A_PDU *amg = new A_PDU();
        amg->setType("DATA");
        amg->setId(counter);
        send(amg,"gate1$o");
    }
    else
    {
        A_PDU* pkt = new A_PDU();
        pkt = check_and_cast<A_PDU*> (msg);

        if(pkt->getId() > 100)
        {
            //EV << "All messages sent!\n";
            drop(msg);
            finish();
            delete (msg);
            return;
        }
        else
        {

            if (pkt->getType () == DATA)            //Message received from Application layer of node1
            {
                A_PDU* pkt_ac = new A_PDU();
                pkt_ac->setId(pkt->getId()+1);
                pkt_ac->setType (ACK);
                bubble("Sending ACK to node1");
                send (pkt_ac, "gate1$o");
            }
            else if(pkt->getType () == ACK)       //Message received from application layer of node2
            {
                A_PDU *pkt = new A_PDU();
                pkt->setId(counter);
                pkt->setType(DATA);
                bubble("Sending DATA to node2");
                send(pkt,"gate1$o");
            }


        }

    }
}

