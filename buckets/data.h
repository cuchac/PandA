unsigned set_initial_buckets(void) {
   unsigned b=1;
   ffree=1;
   (vertices+b)->backptr=0;
   (vertices+b)->bucket[0]=0;
   (vertices+b)->bucket[1]=0;
   (vertices+b)->bucket[2]=0;
   (vertices+b)->bucket[3]=0;
   (vertices+b)->bucket[4]=0;
   putvertex(0);

   return b;
}

void set_full_buckets_1() {
   full_buckets[0]=14;
   full_buckets[1]=10;
   full_buckets[2]=6;
   full_buckets[3]=2;
   full_buckets[4]=8;
}

void set_full_buckets_2() {
   full_buckets[0]=15;
   full_buckets[1]=12;
   full_buckets[2]=8;
   full_buckets[3]=4;
   full_buckets[4]=6;
}

void set_full_buckets_3() {
   full_buckets[0]=14;
   full_buckets[1]=10;
   full_buckets[2]=12;
   full_buckets[3]=3;
   full_buckets[4]=8;
}

void set_initial_buckets_1() {
   (vertices+1)->bucket[2]=1;
}

void set_final_buckets_1_1() {
   set_full_buckets_1();

   set_initial_buckets_1();

   final_buckets[0]=12;
   final_buckets[1]=6;
   final_buckets[2]=4;
   final_buckets[3]=1;
   final_buckets[4]=8;
}

void set_final_buckets_1_2() {
   set_full_buckets_1();

   set_initial_buckets_1();

   final_buckets[0]=14;
   final_buckets[1]=4;
   final_buckets[2]=5;
   final_buckets[3]=0;
   final_buckets[4]=4;
}

void set_final_buckets_1_3() {
   set_full_buckets_1();

   set_initial_buckets_1();

   final_buckets[0]=12;
   final_buckets[1]=6;
   final_buckets[2]=6;
   final_buckets[3]=2;
   final_buckets[4]=4;
}

void set_final_buckets_1_4() {
   set_full_buckets_1();

   set_initial_buckets_1();

   final_buckets[0]=0;
   final_buckets[1]=2;
   final_buckets[2]=1;
   final_buckets[3]=2;
   final_buckets[4]=8;
}

void set_final_buckets_2_1() {
   set_full_buckets_2();

   final_buckets[0]=5;
   final_buckets[1]=5;
   final_buckets[2]=5;
   final_buckets[3]=0;
   final_buckets[4]=1;
}

void set_final_buckets_2_2() {
   set_full_buckets_2();

   final_buckets[0]=12;
   final_buckets[1]=1;
   final_buckets[2]=3;
   final_buckets[3]=4;
   final_buckets[4]=5;
}

void set_final_buckets_2_3() {
   set_full_buckets_2();

   final_buckets[0]=11;
   final_buckets[1]=1;
   final_buckets[2]=3;
   final_buckets[3]=4;
   final_buckets[4]=5;
}

void set_final_buckets_2_4() {
   set_full_buckets_2();

   final_buckets[0]=3;
   final_buckets[1]=12;
   final_buckets[2]=4;
   final_buckets[3]=0;
   final_buckets[4]=6;
}

void set_final_buckets_2_5() {
   set_full_buckets_2();

   final_buckets[0]=2;
   final_buckets[1]=0;
   final_buckets[2]=4;
   final_buckets[3]=3;
   final_buckets[4]=6;
}

void set_final_buckets_3_1() {
   set_full_buckets_3();

   final_buckets[0]=13;
   final_buckets[1]=9;
   final_buckets[2]=12;
   final_buckets[3]=2;
   final_buckets[4]=7;
}

void set_final_buckets_3_2() {
   set_full_buckets_3();

   final_buckets[0]=1;
   final_buckets[1]=5;
   final_buckets[2]=5;
   final_buckets[3]=3;
   final_buckets[4]=4;
}

void set_final_buckets_3_3() {
   set_full_buckets_3();

   final_buckets[0]=0;
   final_buckets[1]=9;
   final_buckets[2]=6;
   final_buckets[3]=3;
   final_buckets[4]=1;
}

void set_final_buckets_3_4() {
   set_full_buckets_3();

   final_buckets[0]=12;
   final_buckets[1]=0;
   final_buckets[2]=12;
   final_buckets[3]=0;
   final_buckets[4]=2;
}

void set_final_buckets_3_5() {
   set_full_buckets_3();

   final_buckets[0]=7;
   final_buckets[1]=3;
   final_buckets[2]=7;
   final_buckets[3]=0;
   final_buckets[4]=0;
}

void set_final_buckets_3_6() {
   set_full_buckets_3();

   final_buckets[0]=7;
   final_buckets[1]=0;
   final_buckets[2]=7;
   final_buckets[3]=0;
   final_buckets[4]=7;
}
