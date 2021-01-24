class Room {
  boolean infected;
  boolean full;
  int xpos;
  int ypos;
  int capacity;
  int numPatients;
  
  Room(int x, int y, int cap, int pat){
    xpos = x;
    ypos = y;
    capacity = cap;
    numPatients = pat;
    if (pat == cap) {
      full = true;
    }
    else {
      full = false;
    }
  }
  
  boolean addPatient(boolean infected){
    if (full){
      System.out.println("Room is at capacity.");
      return false;
    }
    
    else if (infected){ // all other patients in the room become infected
      this.infected = true; 
    }
    
    numPatients += 1;
    if (numPatients == capacity){
      full = true;
    } 
    return true;
  }
}
