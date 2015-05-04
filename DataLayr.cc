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

#include "DataLayr.h"
#include<queue>

Define_Module(DataLayr);

void DataLayr::initialize()
{
    // TODO - Generated method body
        nodeid = par("nodeid");
        k = par("k");
        counter=0;
        timeout = 5 * 0.1 + 0.1; // 2 * propagation time + transmission time
        timeoutEvent = new cMessage("timeout event");
        first = last = 0;
        rn = 0;

        g1i = gate("gate1$i");
        g1o = gate("gate1$o");
        g2i = gate("gate2$i");
        g2o = gate("gate2$o");


}

void DataLayr::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    counter++;
    //IF message is from App layer

    if (msg == timeoutEvent)
    {
        // Timeout event
        EV << "Timeout has reached. Retransmitting from first";

        // Extract the stored message from window
        DL_PDU* storedMessage = myVector[first];

        // Bring back sn to sf
        last = first;

        // Retransmit
        send(storedMessage->dup(), g2o);
        scheduleAt(simTime() + timeout, timeoutEvent);

        return;
    }

    if(msg->getArrivalGate() == g1i)
    {
        DL_PDU *dlpkt = new DL_PDU();
        //Sender's node
        if(nodeid==1)
        {
            // check window size,don't send if full
            if (abs(last - first) == k)
            {
                return;
            }

            dlpkt->setSc(1);        //compound module node1 has id =1 and node2 has id=2
            dlpkt->setDst(2);
            dlpkt->setId(id);
            id = ((id + 1) % k);

            dlpkt->encapsulate(check_and_cast<A_PDU*>(msg));

            // Push the message in the vector
            myVector.push_back(dlpkt);
            if(uniform(0,1)<0.3)
            {

                sendDelayed(dlpkt,simTime()+2.0,"gate2$o");
                dlpkt->setTimestamp();
            }
            else
            {

                sendDelayed(dlpkt,simTime()+1.0,"gate2$o");
                dlpkt->setTimestamp();
            }
            last++;

            // Now if window is full, initialize a timer
            if (abs(last - first) == k) {
                EV << "Setting the timer";
                scheduleAt(simTime() + timeout, timeoutEvent);
            }
        }
        //Receivers's Node
        else if(nodeid==2)
        {
            dlpkt->setSc(2);        //compound module node1 has id =1 and node2 has id=2
            dlpkt->setDst(1);

            // Ask for next id since it is ARQ
            rn = ((rn + 1) % k);
            dlpkt->setId(rn);
            EV << "Receiver window: " << rn;
            dlpkt->encapsulate(check_and_cast<A_PDU*>(msg));

            if(uniform(0,1)<0.4)
                sendDelayed(dlpkt,simTime()+2.0,"gate2$o");
            else
                sendDelayed(dlpkt,simTime()+1.0,"gate2$o");
        }

    }
    else  //If message is from phy layer
    {
        DL_PDU* p = check_and_cast<DL_PDU*>(msg);

        if (nodeid == 1)
        {
            // Sender's node (Ack is received)
            // As soon as ACK is received cancel the timer
            // since the out of order will be handled by go back-n

            cancelEvent(timeoutEvent);

            // Read message stored at sf
            DL_PDU* sm = myVector[first];
            int expectedId = (sm->getId() + 1) % k;

            EV << "Expecting ack: " << expectedId << "\n";

            if (expectedId != p->getId())
            {
                // i.e either negative Ack or not in order Ack
                EV << "Negative / out of order ACK received. Retransmitting";
                last = first;

                // Send the stored message to Physical Layer
                send(sm->dup(), g2o);
            }
            else
            {
                EV << "RTT is: " << (simTime() - p->getTimestamp()) << "\n";
                EV << "Throughput is: " << (first / simTime());

                // All is well
                first++;

                // Forward the packet to App Layer
                send(check_and_cast<A_PDU*>(p->decapsulate()), g1o);
            }
        }
        else
        {
            // Receiver's  node
            if (rn != p->getId())
            {
                // wrong data message
                EV << "data message received is not correct \n";

                if (rn < p->getId()) {
                    // Received ID higher than expected.
                    // Possibly some packet loss. Ask for retransmission

                    EV << "Received data with greater ID. Asking for: " << rn;
                    DL_PDU* re = new DL_PDU(); // Create a new message asking for retransmission
                    re->setSc(2);
                    re->setDst(1);
                    re->setId(rn);
                    send(re, g2o);
                }
                else
                {
                    // Receiver already received this packet before,reset the receiver window to maintain the order

                    EV << "Received data with ID less than rn";
                    rn = p->getId();
                    send(check_and_cast<A_PDU*>(p->decapsulate()), g1o);
                }

            }
            else
            {
                EV << "Response time is: " << (simTime() - p->getTimestamp());
                EV << "Data message received is correct";
                send(check_and_cast<A_PDU*>(p->decapsulate()), g1o);
            }
        }
    }
}
