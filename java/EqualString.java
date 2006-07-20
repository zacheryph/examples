/* EqualString.java
 *
 * according to someone you can go string == string
 */

public class EqualString
{
	public static void main(String args[]) {
		String str1 = "some string";
		String str2 = "some string";
		String str3 = "Some StrinG";
		String str4 = str3.toLowerCase();

		System.out.println("str1 == str2: "+(str1 == str2));
		System.out.println("str1 == str2.toLowerCase(): "+(str1 == str2.toLowerCase()));
		System.out.println("str1 == str3.tolower: "+(str1 == str3.toLowerCase()));
		System.out.println("str1 == str4: "+(str1 == str4));
		System.out.println("str1.equals(str4): "+(str1.equals(str4)));
		
		return;
	}
}

