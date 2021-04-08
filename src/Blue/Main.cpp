#include "BlueAgent.h"
#include <iostream>
#include <unistd.h>
#include <tuple>
#include <string>
#include "../Green/GreenNodeTypes.h"
int main(){
	int pe[2];
	int ps[2];
	 neighbors_identity_t vector;
	 for (int i = 0; i < 100 ;i++){
		 green_node_identity_t nodId;
		 nodId.id = i;
		 vector.push_back(nodId);
	 }
	 
	BlueAgent n;
	n.setPipes();
	n.setGreenId(344);
	n.receiveNeighborsInfo(vector);		
	 n.establishConnectionWithNode();
	 n.sendNeighbors();		
			n.sendToBlueNode((char*)"Hola, amigo.",256);
			n.sendToBlueNode((char*)"como esta?",256);
			n.sendToBlueNode((char*)"bien?",256);
			int i=0;
			
			while(i<5){
				n.listenToBlueNode();
				i++;
			}
			
					n.sendToBlueNode((char*)"se acabo",256);
					std::tuple<int,int,std::string> m1=n.popMessageFromQueue();
					std::cout<<std::get<0>(m1)<<" "<<std::get<1>(m1)<<" "<<std::get<2>(m1)<<"   "<<n.isYourQueueEmpty()<<std::endl;
					std::tuple<int,int,std::string> m2=n.popMessageFromQueue();
					
					
					std::cout<<std::get<0>(m2)<<" "<<std::get<1>(m2)<<" "<<std::get<2>(m2)<<"   "<<n.isYourQueueEmpty()<<std::endl;
					
					std::tuple<int,int,std::string> m3=n.popMessageFromQueue();
					std::cout<<std::get<0>(m3)<<" "<<std::get<1>(m3)<<" "<<std::get<2>(m3)<<"   "<<n.isYourQueueEmpty()<<std::endl;
					
					std::tuple<int,int,std::string> m4=n.popMessageFromQueue();
					std::cout<<std::get<0>(m4)<<" "<<std::get<1>(m4)<<" "<<std::get<2>(m4)<<"   "<<n.isYourQueueEmpty()<<std::endl;
					
										std::tuple<int,int,std::string> m5=n.popMessageFromQueue();
					std::cout<<std::get<0>(m5)<<" "<<std::get<1>(m5)<<" "<<std::get<2>(m5)<<"   "<<n.isYourQueueEmpty()<<std::endl;
			n.fin();
	

	
}



