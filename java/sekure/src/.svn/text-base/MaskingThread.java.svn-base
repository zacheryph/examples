import java.io.*;

/**
 * This class attempts to erase characters echoed to the console.
 */

class MaskingThread extends Thread {
   private boolean stop = false;
   private int index;
   private String prompt;


  /**
   *@param prompt The prompt displayed to the user
   */
   public MaskingThread(String prompt) {
      this.prompt = prompt;
   }


  /**
   * Begin masking until asked to stop.
   */
   public void run() {
      while(!stop) {
         try {
            // attempt masking at this rate
            this.sleep(1);
         }catch (InterruptedException iex) {
            iex.printStackTrace();
         }
         if (!stop) {
            System.out.print("\r" + prompt + " \r" + prompt);
         }
         System.out.flush();
      }
      // erase any prompt that may have been spuriously written on 
      // the NEXT line after the user pressed enter
      System.out.print('\r');
      for (int i = 0; i < prompt.length(); i++) System.out.print(' ');
      System.out.print('\r');
   }


  /**
   * Instruct the thread to stop masking.
   */
   public void stopMasking() {
      this.stop = true;
   }
}
