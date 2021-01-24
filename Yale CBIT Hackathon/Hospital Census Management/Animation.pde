//Animation for moving patient
class Animation {
  PImage[] images;
  int imageCount;
  int frame;
  
  Animation(String imagePrefix, int count) {
    imageCount = count;
    images = new PImage[imageCount];

    for (int i = 0; i < imageCount; i++) {
      // Use nf() to number format 'i' into two digits
      String filename = imagePrefix + nf(i, 2) + ".png";
      PImage img = loadImage(filename);
      image(img, 0, 0);
      img.resize(250, 200);
      image(img, 0, 0);
      images[i] = img;
    }
  }

  void display(float xpos, float ypos) {
    frame = (frame+1) % imageCount;
    image(images[frame], xpos, ypos);
  }
  
  int getWidth() {
    return images[0].width;
  }
}
