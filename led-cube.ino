#include "animation.h"

AnimationManager manager;

void setup() {
  pinMode( 2,OUTPUT);
  pinMode( 3,OUTPUT);
  pinMode( 4,OUTPUT);
  pinMode( 5,OUTPUT);
  pinMode( 6,OUTPUT);
  pinMode( 7,OUTPUT);
  pinMode( 8,OUTPUT);
  pinMode( 9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  for(int i = 2; i <= 13; i++) {
    digitalWrite(i,LOW);
  }
  
  AnimSeq * blinkSeq = new AnimSeq(2);
  blinkSeq->addAnim(new AllOnAnim(250));
  blinkSeq->addAnim(new AllOffAnim(250));
  
  RepeatAnim * blinkAnim = new RepeatAnim(3,blinkSeq);
  
  manager.addAnim(blinkAnim);
  manager.addAnim(new RandMoveAnim(200,16));
}

/** /
void loop() {
  // do nothing
}
/**/
void loop() {
  while(true) {
    manager.runAnimations();
  }
}
/**/
