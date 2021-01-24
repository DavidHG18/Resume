

//Patient decribes individual patient
class Patient {
  Animation anim;
  float xpos;
  float ypos;
  boolean infected;
  
  Patient(Animation a, float xstart, float ystart, float rate) {
    xpos = xstart;
    ypos = ystart;
    anim = a;
    if (random(1) < rate) { //infected based upon real infection rate (different from perceived)
      infected = true;
    }
    else {
      infected = false;
    }
  }
  
  void changePos(float newx, float newy) {
    xpos = newx;
    ypos = newy;
  }
  
  void display() {
    anim.display(xpos, ypos);
  }
}
