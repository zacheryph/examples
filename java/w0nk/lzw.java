import java.io.*;
import java.util.*;

interface BitUtils {
  public static final int BITS_PER_BYTES = 8;

}

// BitInputStream class: Bit-input stream wrapper class.
//
// CONSTRUCTION: with an open InputStream.
//
// ******************PUBLIC OPERATIONS***********************
// int readBit( )              --> Read one bit as a 0 or 1
// void close( )               --> Close underlying stream

class BitInputStream {
  public BitInputStream(InputStream is) {
    in = is;
    bufferPos = BitUtils.BITS_PER_BYTES;
  }

  public int readBit() throws IOException {
    if (bufferPos == BitUtils.BITS_PER_BYTES) {
      buffer = in.read();
      if (buffer == -1)
        return -1;
      bufferPos = 0;
    }

    return getBit(buffer, bufferPos++);
  }

  public void close() throws IOException {
    in.close();
  }

  private static int getBit(int pack, int pos) {
    return (pack & (1 << pos)) != 0 ? 1 : 0;
  }

  private InputStream in;
  private int buffer;
  private int bufferPos;
}

// BitOutputStream class: Bit-output stream wrapper class.
//
// CONSTRUCTION: with an open OutputStream.
//
// ******************PUBLIC OPERATIONS***********************
// void writeBit( val )        --> Write one bit (0 or 1)
// void writeBits( vald )      --> Write array of bits
// void flush( )               --> Flush buffered bits
// void close( )               --> Close underlying stream

class BitOutputStream {
  public BitOutputStream(OutputStream os) {
    bufferPos = 0;
    buffer = 0;
    out = os;
  }

  public void writeBit(int val) throws IOException {
    buffer = setBit(buffer, bufferPos++, val);
    if (bufferPos == BitUtils.BITS_PER_BYTES)
      flush();
  }

  public void writeBits(int[] val) throws IOException {
    for (int i = 0; i < val.length; i++)
      writeBit(val[i]);
  }

  public void flush() throws IOException {
    if (bufferPos == 0)
      return;

    out.write(buffer);
    bufferPos = 0;
    buffer = 0;
  }

  public void close() throws IOException {
    flush();
    out.close();
  }

  private int setBit(int pack, int pos, int val) {
    if (val == 1)
      pack |= (val << pos);
    return pack;
  }

  private OutputStream out;
  private int buffer;
  private int bufferPos;
}

class Element {
  int prefix;
  int suffix;

  public Element(int prefix, int suffix) {
    this.prefix = prefix;
    this.suffix = suffix;
  }
}

public class lzw {
  final static int MAX_CODES = 4096;
  final static int BYTE_SIZE = 8;
  final static int EXCESS = 4;
  final static int ALPHA = 256;
  final static int MASK1 = 255;
  final static int MASK2 = 15;

  static int leftOver;
  static boolean bitsLeftOver;
  static int[] s;
  static int size;
  static Element[] h;

  DataInputStream in;
  //BitInputStream bin;
  //BufferedOutputStream out;

  public static void compress(String inFile) throws IOException {

    int c, d;
    String compressedFile = inFile + ".lz";
    InputStream in = new BufferedInputStream(
        new FileInputStream(inFile));
    OutputStream fout = new BufferedOutputStream(
        new FileOutputStream(compressedFile));
    BitOutputStream bout = new BitOutputStream(fout);
    PrintWriter pout = new PrintWriter(new FileWriter(compressedFile));
    // read input file into a byte array stream
    //ByteArrayOutputStream byteOut = new ByteArrayOutputStream();

    in.mark(10000000);
    int charCounter = 0;
    while (in.read() != -1) {
      charCounter++;
    }

    // convert the input file to an array of bytes in memory
    byte[] theInput = new byte[charCounter]; //byteOut.toByteArray();
    //ArrayList theInput = new ArrayList();
    in.reset();
    in.read(theInput);
    int currenti;

    byte[] theInputCopy = new byte[charCounter];

    Vector rules = new Vector();
    String rule;
    theInputCopy[0] = theInput[0];
    theInputCopy[1] = theInput[1];
    /*   theInputCopy[2]=theInput[2];
        for(int i=2;i<charCounter;i++){
          for (currenti=1;currenti<(i);currenti++){
            if ( ((new String(theInput,(i-1),1))+(new
String(theInput,(i),1))).equals((new String(theInput,(currenti-1),1))+(new
String(theInput,(currenti),1)))){
             //System.out.println("i:"+(theInput[i-1])+"
curi:"+theInputCopy[currenti-1]);

     rule=(" "+new String(theInput, (i - 1), 1)) +(new String(theInput, (i),
1));

             System.out.print(rule);
             //rules.add();

           }

                   // System.out.print(" "+(new String(theInput,(i-1),1))+(new
String(theInput,(i),1))+" TO "+(new String(theInput,(currenti-1),1))+(new
String(theInput,(currenti),1))+";");

          }
        }*/

    in.reset();

    Hashtable table = new Hashtable();
    for (int i = 0; i < ALPHA; i++)
      table.put(new Integer(i), new Integer(i));

    int codeUsed = ALPHA;

    int p = in.read();
    if (p != -1) {
      int pcode = p;
      p = in.read();
      while (p != -1) {
        int k = (pcode << BYTE_SIZE) + p;
        Integer e = (Integer) table.get(new Integer(k));
        if (e == null) {

          if (bitsLeftOver) {
            d = pcode & MASK1;
            c = (leftOver << EXCESS) + (pcode >> BYTE_SIZE);
            pout.print(c);
            pout.print(d);
            bitsLeftOver = false;
          }
          else {
            leftOver = pcode & MASK2;
            c = pcode >> EXCESS;
            pout.print(c);
            bitsLeftOver = true;
          }

          if (codeUsed < MAX_CODES)
            table.put(new Integer( (pcode << BYTE_SIZE) + p),
                      new Integer(codeUsed++));
          pcode = p;
        }
        else pcode = e.intValue();
        p = in.read();
      }

      if (bitsLeftOver) {
        d = pcode & MASK1;
        c = (leftOver << EXCESS) + (pcode >> BYTE_SIZE);
        pout.print(c);
        pout.print(d);
        bitsLeftOver = false;
      }
      else {
        leftOver = pcode & MASK2;
        c = pcode >> EXCESS;
        pout.print(c);
        bitsLeftOver = true;
      }

      if (bitsLeftOver)
        pout.print(leftOver << EXCESS);
    }

//    PrintWriter pout = new PrintWriter(new FileWriter(compressedFile));
//    pout.print(new String(theInput));

    pout.close();
    in.close();
    fout.close();

  }

  /*  private static void output(int pcode) throws IOException{
      int c,d;
      if(bitsLeftOver){
          d = pcode & MASK1;
          c = (leftOver << EXCESS)+(pcode>>BYTE_SIZE);
          out.write(c);
          out.write(d);
          bitsLeftOver = false;
      }
      else{
          leftOver = pcode & MASK2;
          c = pcode>>EXCESS;
          out.write(c);
          bitsLeftOver = true;
      }
   }*/

  public void uncompress(String compressedFile) throws IOException {
    String inFile;
    String extension;

    inFile = compressedFile.substring(0, compressedFile.length() - 3);
    extension = compressedFile.substring(compressedFile.length() - 3);

    if (!extension.equals(".lz")) {
      System.out.println("Not a compressed file!");
      return;
    }

    inFile += ".uc"; // for debugging, so as to not clobber original
    InputStream fin = new BufferedInputStream(
        new FileInputStream(compressedFile));
    in = new DataInputStream(fin);
    BitInputStream bin = new BitInputStream(in);

    OutputStream fout = new BufferedOutputStream(
        new FileOutputStream(inFile));

    int codeUsed = ALPHA;
    s = new int[MAX_CODES];
    h = new Element[MAX_CODES];

    int pcode = getCode() , ccode;
    
    
    
    
    if (pcode >= 0) {
      s[0] = pcode;
      fout.write(s[0]);
      size = 0;

      do {
        ccode = getCode();
        if (ccode < 0)break;
        if (ccode < codeUsed) {
          
          
          size = -1;
          while (ccode >= ALPHA) {
            s[++size] = h[ccode].suffix;
            ccode = h[ccode].prefix;
          }
          s[++size] = ccode;
          for (int i = size; i >= 0; i--)
            fout.write(s[i]);
                            
          
          if (codeUsed < MAX_CODES)
            h[codeUsed++] = new Element(pcode, s[size]);
        }
        
        
        else {
          h[codeUsed++] = new Element(pcode, s[size]);
          
          
          
          size = -1;
          while (ccode >= ALPHA) {
            s[++size] = h[ccode].suffix;
            ccode = h[ccode].prefix;
          }
          s[++size] = ccode;
          for (int i = size; i >= 0; i--)
            fout.write(s[i]);

          
          
          
        }
        pcode = ccode;
      }
      while (true);
    }

    bin.close();
    fout.close();
  }

  private int getCode() throws IOException {
    int c = in.read();
    if (c == -1)return -1;

    int code;
    if (bitsLeftOver)
      code = (leftOver << BYTE_SIZE) + c;
    else {
      int d = in.read();
      code = (c << EXCESS) + (d >> EXCESS);
      leftOver = d & MASK2;
    }
    bitsLeftOver = !bitsLeftOver;
    return code;
  }

/*  private void output(int code) throws IOException {
    size = -1;
    while (code >= ALPHA) {
      s[++size] = h[code].suffix;
      code = h[code].prefix;
    }
    s[++size] = code;
    for (int i = size; i >= 0; i--)
      out.write(s[i]);
  }*/

  public static void main(String[] args) throws IOException {

    lzw uncompress = new lzw();

    if (args.length < 2) {
      System.out.println("Usage: java lzw -[cu] files");
      return;
    }

    String option = args[0];
    for (int i = 1; i < args.length; i++) {
      String nextFile = args[i];
      if (option.equals("-c"))
        compress(nextFile);
      else if (option.equals("-u"))
        uncompress.uncompress(nextFile);
      else {
        System.out.println("Usage: java lzw -[cu] files");
        return;
      }
    }
  }

}

