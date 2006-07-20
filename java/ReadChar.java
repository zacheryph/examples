/* ReadChar: print value of char's as you type them in
 */
import java.io.IOException;

public class ReadChar {
	public ReadChar() {
	}

	public void doWork() {
		int cur = 0;
		while (true) {
			try {
				cur = System.in.read();
			}
			catch (IOException ex) {
				System.err.println("Error: "+ex);
				System.exit(-1);
			}

			System.out.println("char #: "+cur);
		}
	}

	public static void main(String args[]) {
		ReadChar rc = new ReadChar();
		rc.doWork();
	}
}

