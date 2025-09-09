#include <CGRA/Scheduler.h>


Scheduler::Scheduler(OpGraph* opGraph, std::string arch_name)
 : opGraph(opGraph) {
    
    //Get supported ops
    std::string network_latency_filename;
    std::string cgrame_rootdir = "CGRA_ME_ROOTDIR";
    std::string dir = std::getenv(cgrame_rootdir.c_str());
    dir = dir + "/src/sched/" + arch_name + "_supported_ops.dot";
    network_latency_filename = dir;
    parse_latency_file(network_latency_filename);

    //Initialize and schedule asap
    asap_schedule = initializeSched();
    bool asapSucc = calculateASAP(); //no resource constraints
    if(asapSucc){
        std::cout << "Scheduling complete!\n";
    }else{
         throw cgrame_error("Could not create ASAP schedule!\n");
    }


}

/*General scheduling*/
//Reschedule a store when an alias edge is violated
std::vector<OpGraphOp*> Scheduler::rescheduleStore(OpGraphOp *load, OpGraphOp *store, int dist){
  //Current distance between load store - alias ege + 1
    std::vector<OpGraphOp*> rescheduledOps;
    if(!alap_schedule.empty()){
        int delay = alap_schedule.at(store)
            - alap_schedule.at(load) - dist + 1;
        alap_schedule[store] = alap_schedule.at(store)+delay;
        // return verifyBoundsALAP(store);
    }else{
        int delay = asap_schedule.at(store)
        - asap_schedule.at(load) - dist + 1;
        asap_schedule[store] = asap_schedule.at(store)+delay;
        rescheduledOps = verifyBoundsASAP(store);
        return rescheduledOps;
    }
    return rescheduledOps;
}

std::vector<OpGraphOp*> Scheduler::reschedulePredicateEdge(OpGraphOp *pred, OpGraphOp *output){
  //TargetOp will be moved to the new location
  OpGraphOp *targetOp;
  std::vector<OpGraphOp*> rescheduledOps;
  int newStart;
  //Find new location for both ops
  if(alap_schedule.empty()){
    newStart = std::max(asap_schedule[pred],asap_schedule[output]);
    if(asap_schedule[pred] == newStart){
        targetOp = output;
    }else{
        targetOp = pred;
    }
  }else{
    newStart = std::max(alap_schedule[pred],alap_schedule[output]);
    if(alap_schedule[pred] == newStart){
        targetOp = output;
    }else{
        targetOp = pred;
    }
  }
  if(alap_schedule.empty()){
    asap_schedule[targetOp] = newStart;
    return verifyBoundsASAP(targetOp);
  }else{
    alap_schedule[targetOp] = newStart;
    // return verifyBoundsALAP(targetOp);
  }
}
bool hasBackedge(OpGraphOp* phi, OpGraphOp* inputOp){
    std::cout << "Checking back edges: Phi " << *phi << " input: "<< *inputOp <<"\n";
    if(*inputOp == *phi){
        return true;
    }
    if(inputOp->input.size() == 0 || inputOp->opcode == OpCode::PHI){
        return false;
    }
    bool foundBackedge = false;
    for(int i = 0; inputOp && i < (std::ptrdiff_t)inputOp->input.size(); i++) // for all node inputs
    {
        OpGraphVal* in = inputOp->input[i];
        OpGraphOp* inOp = in->input;
        if(hasBackedge(phi,inOp)){
            foundBackedge = true;
            break;
        }

    }
    return foundBackedge;
}
/*ASAP scheduling*/
int Scheduler::scheduleASAPOperation(OpGraphOp* op){
    int latestStart = 0;
    for(int i = 0; op && i < (std::ptrdiff_t)op->input.size(); i++) // for all node inputs
    {
        OpGraphVal* in = op->input[i];
        //operation backedges
        if(*(in->input) == *op){
            continue;
        }

    //Skip backedges with a phi
    if (op->opcode == OpCode::PHI) {
        bool skipInput = false; // Flag variable for continuing the outer loop
        for (int i = 0; op && i < static_cast<std::ptrdiff_t>(in->input->input.size()); i++) // for all node inputs
        {
            if(hasBackedge(op,in->input)){
                skipInput = true;
                break;
            }
            OpGraphVal* in2 = in->input->input[i];
            if (*(in2->input) == *op) {
                skipInput = true; // Set the flag to continue the outer loop
                break; // Break out of the inner loop
            }
        }
        if (skipInput) {
            continue; // Continue the outer loop
        }
    }

        //Check if input is scheduled:
        if(asap_schedule.at(in->input) == -1)
        {
            std::cout << "Not scheduled: " << *(in->input) << "\n";
            return -1;
        }

        //Find latest start:
        latestStart = std::max(latestStart,asap_schedule.at(in->input) +
                                getLowerBound((in->input)->opcode,op->opcode)
                                + op_latency[(in->input)->opcode]);
    }

        asap_schedule[op] = latestStart;
    //Check upperbounds for consumers (if scheduled)
    //verify consumers do not violate any bounds
    if(op->output != NULL && op->output->output.size() > 0){
    
        for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
        {
            OpGraphOp* consumer = op->output->output[i];
            int wait = asap_schedule[consumer] - (asap_schedule[op]);
            if(asap_schedule[consumer] != -1){
                if(wait > getUpperBound(op->opcode,consumer->opcode)){
                    std::vector<OpGraphOp*> lockedOps;
                    rescheduleForward(op,consumer,&lockedOps);
                }
            }
        }
    }
    return asap_schedule[op];
}

int Scheduler::unscheduleASAP(OpGraphOp* op, std::vector<OpGraphOp*>* rescheduledOps) {
  if(asap_schedule.at(op) == -1){
      return 0;
  }
  //Check list to see if op has been recently locked by rescheduling:
  auto check = std::find(rescheduledOps->begin(), rescheduledOps->end(), op);
  assert(check == rescheduledOps->end() && "Unable to resolve upperbound dependecies.");
  
  std::cout << "Unscheduling: " << *op << "\n";

  // Unschedule the operation
    int unscheduledOps = 0;
    
    // freeResource(op,asap_schedule[op]%ASAPII);
    asap_schedule[op] = -1;
    rescheduledOps->push_back(op);
    // //Get rescheduled start to calculation if other consumers are bound
    // //Do not unschedule consumer operations not bound by rescheduled producer

    unscheduledOps++;
    // Unscheduling the dependencies
    if(op->output == NULL || op->output->output.size() == 0){
        std::cout << "No outputs!\n";
        return unscheduledOps;
    }
    for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
    {
        OpGraphOp* consumer = op->output->output[i];
        std::cout << "Consumer " << *consumer << " at " << asap_schedule[consumer]<< "\n";
        if(asap_schedule.at(consumer) != -1){
            unscheduledOps += unscheduleASAP(consumer,rescheduledOps);
        }
    }
    return unscheduledOps;
}

/*
Reschedule operations that violate upperbounds during ASAP scheduling.
Operations found to violate upper bounds must be scheduled closer to the consumer.
*/
int Scheduler::rescheduleForward(OpGraphOp* op, OpGraphOp* consumer,std::vector<OpGraphOp*>* rescheduledOps) {
  if(asap_schedule.at(op) == -1){
      return 0;
  }
  
  int unscheduledOps = 0;
  int newStart = asap_schedule.at(consumer) - getUpperBound(op->opcode,consumer->opcode);
  //Add to list of rescheduled operations.

    //Free resources (if consumed)
    // freeResource(op,asap_schedule[op]%ASAPII);

    asap_schedule[op] = newStart;
    //Ensure newStart doesn't now violate lowebound after resource check:
    if(newStart > asap_schedule[consumer] - getLowerBound(op->opcode,consumer->opcode)){
        //Allow the unscheduling of consumer in this case
        auto it = std::find(rescheduledOps->begin(), rescheduledOps->end(), consumer);
        if(it != rescheduledOps->end())
            rescheduledOps->erase(it);

        unscheduledOps += unscheduleASAP(consumer,rescheduledOps);
    }

  rescheduledOps->push_back(op);
  
  std::cout << "Rescheduling "<< *op <<" at: " << newStart << "\n";
  //Ensure newstart doesn't violate other scheduled consumers.
  //(consumers are now closer)
    for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
    {
        OpGraphOp* otherConsumer = op->output->output[i];
        if(otherConsumer != consumer && asap_schedule.at(otherConsumer) != -1){
            // LLVM_DEBUG(llvm::dbgs() << "Checking other consumers: " << otherConsumer->getName() << "\n");
            if((newStart + getLowerBound(op->opcode,otherConsumer->opcode)) > asap_schedule.at(otherConsumer)){
                //Check if operation can be legally rescheduled.
                unscheduledOps +=unscheduleASAP(otherConsumer,rescheduledOps);
            }
        }
    }
//   LLVM_DEBUG(llvm::dbgs() << "Done Checking other consumers: " << "\n");
    int wait;
    for(int i = 0; op && i < (std::ptrdiff_t)op->input.size(); i++) // for all node inputs
    {
        OpGraphVal* in = op->input[i];
        //latest start - producer end time
        wait = newStart - (asap_schedule.at(in->input));
        if(wait > getUpperBound(in->input->opcode,op->opcode)){
            //Walk up producers and reschedule if nessasary
            //reduce num_scheduled if operations get unscheduled.
            std::vector<OpGraphOp*>* lockedOps;
            lockedOps->push_back(op);
            unscheduledOps += rescheduleForward(in->input,op,lockedOps);
        }
    }
  return unscheduledOps;
}

std::vector<OpGraphOp*> Scheduler::verifyBoundsASAP(OpGraphOp* op){
    std::cout << "Verifying bounds for " << *op << "\n";
    int wait;
    int latestStart = asap_schedule.at(op);
    std::vector<OpGraphOp*>* lockedOps = new std::vector<OpGraphOp*>;
    //Inputs have no operands
    for(int i = 0; op && i < (std::ptrdiff_t)op->input.size(); i++) // for all node inputs
    {
        OpGraphVal* in = op->input[i];
        //latest start - producer end time
        wait = latestStart - (asap_schedule.at(in->input));
        if(wait > getUpperBound(in->input->opcode,op->opcode)){
            //Walk up producers and reschedule if nessasary
            //reduce num_scheduled if operations get unscheduled.
            lockedOps->push_back(op);
            rescheduleForward(in->input,op,lockedOps);
        }
    }

    //Check upperbounds for consumers (if scheduled)
    //verify consumers do not violate any bounds
    if(op->output != NULL && op->output->output.size() > 0){
    
        for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
        {
            OpGraphOp* consumer = op->output->output[i];
            wait = asap_schedule[consumer] - (asap_schedule[op]);
            // std::cout << "Verifying consumer " << *consumer << "\n";
            if(asap_schedule[consumer] != -1){
                // std::cout << "Checking upperbound 1: " << op->opcode << " -> " << consumer->opcode << "\n";
                if(wait > getUpperBound(op->opcode,consumer->opcode)){
                    // std::cout << "Upper bound violated!\n";
                    rescheduleForward(op,consumer,lockedOps);
                }
            }
        }
    }
    return *lockedOps;
}


std::unordered_map<const OpGraphOp*, int> Scheduler::initializeSched(){
    std::unordered_map<const OpGraphOp*, int> sched;
    // initialize all nodes as -1 (unscheduled)
    for (auto& op : opGraph->opNodes())
    {
        sched[op] = -1;
    }
    return sched;
}

bool Scheduler::calculateASAP() {
    std::cout << "Calculating ASAP!\n";
    int num_scheduled = 0;
    int max_latency = 0;
    unsigned int iterations = 0;

    // create a copy of the list of all op nodes
    auto allNodes = opGraph->opNodes();
    
    //Find number already scheduled
    for(OpGraphOp* op : allNodes){
        if(asap_schedule[op] != -1){
            num_scheduled++;
        }
    }

    while((size_t)num_scheduled< allNodes.size()){
    for(auto&op : allNodes){
        //skip if already scheduled:
        if(asap_schedule.at(op) != -1){
            continue;
        }

        std::cout << "Visiting op '" << *op << "' with "
            << std::to_string(op->input.size()) << " operands:\n";
        

        int latestStart = scheduleASAPOperation(op);
        std::cout << "Latestest start: " << latestStart << "\n";
        if(latestStart != -1){
        asap_schedule[op] = latestStart;
        num_scheduled++;
        //Verify upperbound latency
        
        std::vector<OpGraphOp*> rescheduledOps = verifyBoundsASAP(op);
        int rescheduledOpsCount = rescheduledOps.size();
        std::cout << "Rescheduled ops: \n";
        for(OpGraphOp* op : rescheduledOps){
            if(asap_schedule[op] == -1){
                num_scheduled--;
            }            
        }
        std::cout << "Number scheduled: " << num_scheduled << "\n";
        
        //Verify alias edges
        //Confirm that source -> dest are both scheduled.
        for(auto& alias : opGraph->aliasNodes()){
            OpGraphOp* source = alias->input;
            OpGraphOp* dest = alias->output[0];
            //get dist
            int dist = opGraph->getDist(opGraph->outEdges(source)[0]);

            //Ensure both are scheduled:
            if(asap_schedule.at(source) != -1 && asap_schedule.at(dest) != -1){
                if(asap_schedule.at(dest) - asap_schedule.at(source) <= dist){
                    int rescheduledOpsCount = rescheduleStore(dest,source,dist).size();
                    num_scheduled -= (rescheduledOpsCount == 1 || rescheduledOpsCount == 0) ? 0 : (rescheduledOpsCount-1);
                }
            }
        }
        
        //Check for predicate-linked operations (i.e output & output_pred)
        if(op->opcode == OpCode::OUTPUT_PRED || op->opcode == OpCode::OUTPUT){
            //OUTPUT_PRED must be on the same cycle as OUTPUT
            OpCode linkedOpCode = op->opcode == OpCode::OUTPUT_PRED ? OpCode::OUTPUT : OpCode::OUTPUT_PRED;
            for(OpGraphOp* otherOp : opGraph->opNodes()){
                if(asap_schedule[otherOp] != -1 && otherOp->opcode == linkedOpCode){
                    //Ensure on same cycle:
                    if(asap_schedule[op] != asap_schedule[otherOp]){
                        std::cout << "Rescheduling pred edge!\n";
                        int rescheduledOpsCount = reschedulePredicateEdge(op,otherOp).size();
                        num_scheduled -= (rescheduledOpsCount == 1 || rescheduledOpsCount == 0) ? 0 : (rescheduledOpsCount-1);
                    }
                }
            }
        }
        max_latency=std::max(max_latency,asap_schedule[op]); //max start time
        }
    }
    std::cout << "ASAP Scheduling total :" << allNodes.size() << " Scheduled: "<< num_scheduled << "\n";
    iterations++;
    }
    std::cout << "ASAP Scheduling complete!\n";

    max_cycles = max_latency;
    return true;
}

//Attempt to reschedule op at cycle without having to modify fixed schedules
bool Scheduler::rescheduleASAP(OpGraphOp* op, int cycle){

    //Rescheduling sucesfully is impossible
    if(fixed_schedule[op] != -1 && fixed_schedule[op] != cycle)
    {
        std::cout << "Rescheduling is not possible as " << *op << " is already constrained at" << fixed_schedule[op] << "\n";
        return false;
    }
    std::vector<OpGraphOp*> rescheduledOps;

    //Schedule the op at cycle
    asap_schedule[op] = cycle;

    bool rollBack = false;
    //Check lowerbounds of consumer
    if(op->output != NULL && op->output->output.size() > 0){
        for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
        {
            OpGraphOp* consumer = op->output->output[i];
            if((op->opcode == OpCode::ADD && consumer->opcode == OpCode::PHI) || consumer == op)
            {
                continue;
            }

                // LLVM_DEBUG(llvm::dbgs() << "Checking other consumers: " << otherConsumer->getName() << "\n");
            if((asap_schedule[op] + getLowerBound(op->opcode,consumer->opcode)) > asap_schedule.at(consumer)){
                //Check if operation can be legally rescheduled.
                unscheduleASAP(consumer,&rescheduledOps);
            }
        }
    }else{
        std::cout << *op << " has no consumers!\n";
    }

    for(OpGraphOp* op : rescheduledOps){
        if(fixed_schedule[op] != -1 && fixed_schedule[op] != asap_schedule[op]){
            //Roll back unschedule
            rollBack = true;
            //Revert
            asap_schedule[op] = fixed_schedule[op];
        }
    }

    rescheduledOps = verifyBoundsASAP(op);
    for(OpGraphOp* op : rescheduledOps){
        if(fixed_schedule[op] != -1 && fixed_schedule[op] != asap_schedule[op]){
            //Roll back unschedule
            rollBack = true;
            //Revert
            asap_schedule[op] = fixed_schedule[op];
        }
    }
    //Ensure that a legal schedule can be found that schedules op at cycle.
    return calculateASAP() && !rollBack && asap_schedule[op] == cycle;

}

/*ALAP Scheduling*/
int Scheduler::unscheduleALAP(OpGraphOp* op) {
  if(alap_schedule[op] == -1){
    return 0;
  }
//   LLVM_DEBUG(llvm::dbgs() << "Unscheduling: " << op->getName() << "\n");
  // Unschedule the operation
  int unscheduledOps = 0;
  alap_schedule[op] = -1;

  unscheduledOps++;
  // Unscheduling the dependencies
    for(int i = 0; op && i < (std::ptrdiff_t)op->input.size(); i++) // for all node inputs
    {
        OpGraphOp* producer = op->input[i]->input;
        if(alap_schedule[producer] != -1) {
            unscheduledOps += unscheduleALAP(producer);
        }
    }
  return unscheduledOps;
}

/*
Reschedule operations that violate upperbounds during ALAP scheduling.
Operations found to violate upper bounds must be scheduled closer to the producer.
*/
int Scheduler::rescheduleBackward(OpGraphOp* op, OpGraphOp* producer) {
  int unscheduledOps = 0;
  //reschedule operation
  int newStart = alap_schedule[producer] + getUpperBound(producer->opcode,op->opcode);
  alap_schedule[op] = newStart;
  
//   LLVM_DEBUG(llvm::dbgs() <<"Rescheduling: " << op->getName() <<" New start found to be: " << newStart << "\n");
  
  //Ensure newstart doesn't violate other scheduled producers.
  //(producers are now closer)
    int wait;
    for(int i = 0; op && i < (std::ptrdiff_t)op->input.size(); i++) // for all node inputs
    {
        OpGraphOp* otherProducer = op->input[i]->input;
        if(producer!= otherProducer && alap_schedule[otherProducer] != -1){
            // LLVM_DEBUG(llvm::dbgs() << "Checking other producers: " << otherProducer->getName() << "\n");
            if((newStart + getLowerBound(otherProducer->opcode,op->opcode)) < alap_schedule[otherProducer]){
            unscheduledOps +=unscheduleALAP(otherProducer);
            }
        }
    }

  //verify consumers do not violate any bounds
    for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
    {
        OpGraphOp* consumer = op->output->output[i];
        if(alap_schedule[consumer] != -1){
        wait = newStart - (alap_schedule[producer]);
        // LLVM_DEBUG(llvm::dbgs() << "Checking consumers: " << consumer->getName() << "\n");
        if(wait > getUpperBound(op->opcode,consumer->opcode)){
            unscheduledOps +=rescheduleBackward(consumer,op);
        }
        }
  }

  return unscheduledOps;
}

int Scheduler::verifyBoundsALAP(OpGraphOp* op){
  int wait;
  int num_scheduled = 0;
  int minEnd = alap_schedule[op]; 

    for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
    {
        OpGraphOp* consumer = op->output->output[i];
        if(alap_schedule[consumer] != -1){
        wait = alap_schedule[consumer] -  (minEnd);
        // LLVM_DEBUG(llvm::dbgs() << "Checking violation for: "<< op->getName() <<
        //     " -> " << consumer->getName() << " wait: "<< wait << " Upperbound: "<< getUpperBound(consumer,op)<<"\n");
        if(wait > getUpperBound(op->opcode,consumer->opcode)){
            // LLVM_DEBUG(llvm::dbgs() << "Upper bound violated by: "<< op->getName() <<
            // " -> " << consumer->getName() << "\n");
            num_scheduled -= rescheduleBackward(consumer,op);
        }
        }
  }
  return num_scheduled;
}

int Scheduler::scheduleALAPOperation(OpGraphOp* op){
    int minEnd = std::numeric_limits<int>::max();
    for(int i = 0; op && i < (std::ptrdiff_t)op->output->output.size(); i++) // for all node inputs
    {
        OpGraphOp* consumer = op->output->output[i];
        
        //ignore operation backedges
        if(consumer == op){
            continue;
        }

        if((consumer->opcode == OpCode::PHI && op->opcode == OpCode::ADD))
        {   
            return max_cycles;
        }

        if(alap_schedule.at(consumer) != -1){
            return std::min(minEnd, alap_schedule.at(consumer) - getLowerBound(op->opcode,consumer->opcode));
        }else{
            // LLVM_DEBUG(llvm::dbgs() << "ALAP skipping!" << "\n");
            return -1;
        }
    }
    return -1;
}
bool Scheduler::calculateALAP() {
    int num_scheduled = 0;
    unsigned int iterations = 0;

    if(max_cycles == -1){
        return false;
    }
    // create a copy of the list of all op nodes
    auto allNodes = opGraph->opNodes();
    for(auto&op : allNodes){
        std::cout << "Visiting op '" << *op << "\n";
        //No users
        if(op->output == NULL || op->output->output.size() == 0){
            alap_schedule[op] = max_cycles;
            num_scheduled++;
            std::cout << "Scheduling op " << op->getName() << " at "<<max_cycles <<"\n";
        }else{
            alap_schedule[op] = -1;
        }
    }
    //Set initial value of result to advance.
    while((size_t)num_scheduled < allNodes.size()){
        for(auto&op : allNodes){
            //skip if already scheduled:
            if(alap_schedule.at(op) != -1){
                continue;
            }

            std::cout << "Visiting op '" << *op << "' with "
            << std::to_string(op->input.size()) << " users:\n";
        

            int minEnd = scheduleALAPOperation(op);

            if(minEnd != -1){
            alap_schedule[op] = minEnd;
            num_scheduled++;

            
            //Verify upperbound latency
            num_scheduled += verifyBoundsALAP(op);

            //Verify alias edges
            //Confirm that source -> dest are both scheduled.
            for(auto& alias : opGraph->aliasNodes()){
                OpGraphOp* source = alias->input;
                OpGraphOp* dest = alias->output[0];
                //get dist
                int dist = opGraph->getDist(opGraph->outEdges(source)[0]);

                //Ensure both are scheduled:
                if(alap_schedule.at(source) != -1 && alap_schedule.at(dest) != -1){
                    if(alap_schedule.at(dest) - alap_schedule.at(source) <= dist){
                        int rescheduledOpsCount = rescheduleStore(dest,source,dist).size();
                        num_scheduled -= (rescheduledOpsCount == 1 || rescheduledOpsCount == 0) ? 0 : (rescheduledOpsCount-1);
                    }
                }
            }

            //Check for predicate-linked operations (i.e output & output_pred)
            if(op->opcode == OpCode::OUTPUT_PRED || op->opcode == OpCode::OUTPUT){
                //OUTPUT_PRED must be on the same cycle as OUTPUT
                OpCode linkedOpCode = op->opcode == OpCode::OUTPUT_PRED ? OpCode::OUTPUT : OpCode::OUTPUT_PRED;
                for(OpGraphOp* otherOp : opGraph->opNodes()){
                    if(alap_schedule[otherOp] != -1 && otherOp->opcode == linkedOpCode){
                        //Ensure on same cycle:
                        if(alap_schedule[op] != alap_schedule[otherOp]){
                            std::cout << "Rescheduling pred edge!\n";
                            int rescheduledOpsCount = reschedulePredicateEdge(op,otherOp).size();
                            num_scheduled -= (rescheduledOpsCount == 1 || rescheduledOpsCount == 0) ? 0 : (rescheduledOpsCount-1);
                        }
                    }
                }
            }

            }

        }
        // LLVM_DEBUG(llvm::dbgs() << "ALAP Scheduling left :" << num_left << " Scheduled: "<< num_scheduled << "\n");
        iterations++;
    }

    return true;
}

bool Scheduler::verifyDependenceConstrainedSchedule(std::unordered_map<const OpGraphOp*, int> sched){
    for(auto&op : opGraph->opNodes()){
        bool correct = true;
        std::cout << "Checking " << *op << " scheduled at " << asap_schedule[op] << "\n";
        for(int i = 0; op && i < (std::ptrdiff_t)op->input.size(); i++) // for all node inputs
        {
            OpGraphVal* in = op->input[i];
            
            //ignore phi backedges and operation backedges
            if((in->input->opcode == OpCode::ADD && op->opcode == OpCode::PHI) || in->input == op)
            {
                continue;
            }

            //Check if input is scheduled:
            if(asap_schedule.at(in->input) == -1)
            {
                return false;
            }

            //check lower bound
            correct = asap_schedule[op] >= (asap_schedule.at(in->input) +
                                    getLowerBound((in->input)->opcode,op->opcode) + op_latency[(in->input)->opcode]);

            if(!correct){
                return false;
            }

            //check upper bound
            int wait = asap_schedule[op] - (asap_schedule[in->input]);
            correct = wait <= getUpperBound(in->input->opcode,op->opcode);

            if(!correct){
                return false;
            }
        }
    }
    return true;
}

void Scheduler::parse_latency_file(std::string network_latency_filename) {
    // Parsing the network schedule file
    ConfigGraph parsed_network_latency_dot;
    auto network_latency_ifstream = std::ifstream(network_latency_filename);
    parsed_network_latency_dot = parseDot(network_latency_ifstream, network_latency_filename);
    for (const auto& v : parsed_network_latency_dot.allNodes()) {
        // Setting up the operation latency
        const auto& v_attrs = parsed_network_latency_dot.attributes(v);
        op_latency.emplace(opcode_from_string(parsed_network_latency_dot.name(v)), v_attrs.getInt("OP_LATENCY"));

        for (const auto& out_edge : parsed_network_latency_dot.outEdges(v)) {
            // Setting up the edges upper and lower latencies
            const auto& e_attrs = parsed_network_latency_dot.attributes(out_edge);
            std::string out_name = parsed_network_latency_dot.name(v);
            std::string in_name = parsed_network_latency_dot.name(parsed_network_latency_dot.target(out_edge));
            OpGraphOpCode out_op_code = opcode_from_string(out_name);
            OpGraphOpCode in_op_code = opcode_from_string(in_name);
            OpCodeEdge op_code_edge = {out_op_code, in_op_code};
            lower_bound_op_code_edge.emplace(op_code_edge, e_attrs.getInt("LOWER_BOUND_NETWORK_LATENCY"));
            upper_bound_op_code_edge.emplace(op_code_edge, e_attrs.getInt("UPPER_BOUND_NETWORK_LATENCY"));
        }
    }
}
