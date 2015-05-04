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

#include "PhyLayr.h"
#include "P_PDU_m.h"
#include "DL_PDU_m.h"
Define_Module(PhyLayr);

void PhyLayr::initialize()
{
    // TODO - Generated method body
    g1i = gate("gate1$i");
    g1o = gate("gate1$o");
    g2i = gate("gate1$i");
    g2o = gate("gate1$o");
}

void PhyLayr::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    if (uniform(0, 1) < 0.01) {
        // Drop the frames with 1% probability
        EV << "Losing message\n";
        bubble("Opps, Message Lost");
        return;
    }
    if (msg->getArrivalGate() == g1i)
    {
        // Message is from DataLink Layer
        P_PDU* mpl = new P_PDU();
        mpl->setTtl(10);
        mpl->encapsulate(check_and_cast<DL_PDU*>(msg));
        //EV << "Sending Encapsulated packet to other node.\n";
        bubble("sending packet to node2");
        send(mpl,"gate2$o");
    }
    else
    {
        // Message is from different Node
        P_PDU* p = check_and_cast<P_PDU*>(msg);
        DL_PDU* mdl = check_and_cast<DL_PDU*>(p->decapsulate());
        //EV << "Sending Decapsulated packet to DataLink layer.\n";
        bubble("sending packet to datalink layer");
        send(mdl, "gate1$o");
    }
}
