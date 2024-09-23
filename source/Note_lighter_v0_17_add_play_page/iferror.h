// iferror.h
//
// by Hippolyte Mounier
// created on 2023-12-28
//
// Goal : manages simple runtime error handling 
// 1. IFERROR(mycondition, mymessage) to check if something is wrong with my variables anywhere in the code
//
// 2. IntegrityChecker class to check if my memory has gone corrupt locally (typically memory corrupted by out of bound write access created by a for loop)


#ifndef IFERROR

  #define IFERROR( errorcondition, text )   \
  {                                         \
    if( errorcondition )                    \
    {                                       \
      Serial.println("ERROR was triggered by your IFERROR condition  (from iferror.h): ");            \
      Serial.println("  TODO: please developer search and correct this code: ");   \
      Serial.println(text);                 \
      Serial.println("Exiting now in forced endless loop");                      \ 
      do { }                                \
      while (true);                         \
    }                                       \
  }

  #define MAGIC_KEY 0xF0F0F0F0
  
  class IntegrityChecker
  {
    private:
    const long key_ = MAGIC_KEY;

    public:
    IntegrityChecker(const char * message)
    {
      Serial.begin(9600);
      Serial.print("Info: your IntegrityChecker object '");
      Serial.print(message);
      Serial.print("' is located at memory adress ");
      Serial.println((long)this);
    }

    void failIfCorrupt()
    {
      if(key_ != MAGIC_KEY)
      {
        Serial.print("ERROR corrupted memory found by your IntegrityChecker.failIfCorrupt (from iferror.h) at : ");
        Serial.println((long)this);
        Serial.println("  TODO: please developer check your code for out of bound memory access of the object at this adress");
        Serial.println("Exiting now in forced endless loop");
        do { } 
        while (true);
      }
    }
  };


  #ifndef DEBUGTXT

    #ifndef NODEBUG
      #define DEBUGTXT(txt)                                    { Serial.println(txt); }
      #define DEBUGVAL(txt, val)                               { Serial.print(txt);  Serial.print("="); Serial.println(val); }
      #define DEBUGVAL2(txt, val, txt2, val2)                  { Serial.print(txt);  Serial.print("="); Serial.print(val); Serial.print(" "); Serial.print(txt2);  Serial.print("="); Serial.println(val2);}
      #define DEBUGVAL3(txt, val, txt2, val2, txt3, val3)      { Serial.print(txt);  Serial.print("="); Serial.print(val); Serial.print(" "); Serial.print(txt2);  Serial.print("="); Serial.print(val2); Serial.print(" "); Serial.print(txt3); Serial.print("="); Serial.println(val3);}
      #define DEBUGVAL4(txt, val, txt2, val2, txt3, val3, txt4, val4)      { Serial.print(txt);  Serial.print("="); Serial.print(val); Serial.print(" "); Serial.print(txt2);  Serial.print("="); Serial.print(val2); Serial.print(" "); Serial.print(txt3); Serial.print("="); Serial.print(val3); Serial.print(" "); Serial.print(txt4); Serial.print("="); Serial.println(val4);}
      #define DEBUGPLOT3(val1, val2, val3)                     { Serial.print(val1); Serial.print(","); Serial.print(val2);  Serial.print(","); Serial.println(val3);}
    #else
      #define DEBUGTXT(txt)                                    { }
      #define DEBUGVAL(txt, val)                               { }
      #define DEBUGVAL2(txt, val, txt2, val2)                  { }
      #define DEBUGVAL3(txt, val, txt2, val2, txt3, val3)      { }
      #define DEBUGVAL4(txt, val, txt2, val2, txt3, val3, txt4, val4)      { }
      #define DEBUGPLOT3(val1, val2, val3)                     { }
    #endif
  #endif

#endif