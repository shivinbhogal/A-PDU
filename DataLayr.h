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

#ifndef __LAB4_DATALAYR_H_
#define __LAB4_DATALAYR_H_

#include <omnetpp.h>
#include "DL_PDU_m.h"
#include "A_PDU_m.h"
#include "P_PDU_m.h"

/**
 * TODO - Generated class
 */
class DataLayr : public cSimpleModule
{
    private:
    int id=0;
    int nodeid;
    int counter;
    cGate *g1i,*g1o,*g2i,*g2o;
    int first,last,rn;
    int k=4;
    std::vector<DL_PDU*> myVector;
    cMessage* timeoutEvent;
    simtime_t timeout;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
