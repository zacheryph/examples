/* CommaAssignment: dont ask
 * Claimed By: w0nk_!~w0nk@ool-18bccc2d.dyn.optonline.net
 *
 * i guess with Borland compiler you can go:
 *
 * int IntFunc() { };
 * int = IntFunc(), int;
 *
 * with a 'newer', or maybe just a real compiler
 * this does not work what so ever due to 2 reasons.
 * int = int, int; is none existent in ANY language,
 * and compiler will fail when ccode doesnt exist.
 *
 * Output:
 *
 *	hostensz@fate (~/src/java)]$ javac CommaAssignment.java
 *	CommaAssignment.java:22: ';' expected
 *	                pcode = getCode(), ccode;
 *	                                 ^
 *	1 error                                                 
 */
public class CommaAssignment {
	public static void main(String args[]) {
		int pcode;
		//int ccode = 23;  // according to w0nk ccode was never initialized

		pcode = getCode(), ccode;

		System.out.println("pcode = " + new Integer(pcode).toString());

		return;
	}

	public int getCode() {
		return 12;
	}
}

