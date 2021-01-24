float REAL_INFECTED_RATE = 0.6;
float PERCEIVED_INFECTED_RATE = 0.3;
float PIR_INIT_WEIGHT = 0;
int NUM_PATIENTS = 40;
int NUM_ROOMS = 5;
int[] LIST_OF_CAPACITIES = {10, 15, 25, 5, 10, 20};

int ANIMATION_WIDTH = 250;
int ANIMATION_HEIGHT = 200;

Room room1;
Room room2;
Room room3;
Room room4;
Patient[] patients;
Room[] rooms;
Animation anim; 
boolean reached_destination = false;
int patient_number = 0;
Room destination;
Patient curr_pat;

void settings(){
  size(1200, 800);
}

void setup(){
  background(255);
  frameRate(24);
  noFill();
  stroke(255, 0, 0);
  anim = new Animation("walking", 29);
  rooms = new Room[NUM_ROOMS];
  for (int i = 0; i < NUM_ROOMS; i++) {
    Room thisroom = new Room(width - 200, i * (height/NUM_ROOMS) + 30, LIST_OF_CAPACITIES[i], 0);
    rooms[i] = thisroom;
  }
  patients = new Patient[NUM_PATIENTS];
  for (int i = 0; i < NUM_PATIENTS; i++) {
    Patient thispat = new Patient(anim, 0, 0, REAL_INFECTED_RATE);
    patients[i] = thispat;
  }
  for (Room room: rooms) {
    rect(room.xpos, room.ypos, height/NUM_ROOMS - 50, height/NUM_ROOMS - 50);
    textSize(32);
    fill(0, 102, 153);
    rect(room.xpos, room.ypos, min(height/NUM_ROOMS - 50, 200), min(height/NUM_ROOMS - 50, 200));
  }  
  curr_pat = patients[patient_number];
  destination = getRoom(rooms, curr_pat);
}

void draw() {
  background(255);
  for (Room room: rooms) {
    rect(room.xpos, room.ypos, min(height/NUM_ROOMS - 50, 200), min(height/NUM_ROOMS - 50, 200));
    textSize(20);
    fill(0, 102, 153);
    text(room.numPatients + "/" + room.capacity, room.xpos + 50, room.ypos - 10);
  }  
  if (patient_number < 40){
    if (reached_destination) {
      patient_number++;
      if (patient_number < 40) curr_pat = patients[patient_number];
      destination = getRoom(rooms, curr_pat);
      reached_destination = false;
    }
    curr_pat.display();
    if (curr_pat.xpos + ANIMATION_WIDTH/4*3 < destination.xpos) {
      curr_pat.xpos += 10;
    }
    if (curr_pat.ypos + ANIMATION_HEIGHT/4 < destination.ypos) {
      curr_pat.ypos += 4;
    }
    if (curr_pat.xpos + ANIMATION_WIDTH/4*3 >= destination.xpos && curr_pat.ypos + ANIMATION_HEIGHT/4 >= destination.ypos) {
      reached_destination = true;
      destination.addPatient(curr_pat.infected);
    }
  }
}

//TBD by model
Room getRoom(Room[] rooms, Patient pat) {
  if (pat.infected) {
    return rooms[0];
  }
  else return rooms[1];
}
