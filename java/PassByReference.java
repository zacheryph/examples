/* PassByReference: how does param passing work in java exactly
 *
 * yes i know java params are passed by reference
 * i just made this to see how they work. ill make it
 * way more extensive later
 */
public class PassByReference
{
	private String str;
	private String str2;
	private String str3;

	public PassByReference()
	{
		str = "string 1";
		str2 = "num two";
		str3 = "three";
	}

	public static void main(String args[])
	{
		String str_1 = "my string 1";
		String str_2 = "another two";
		String str_3 = "my number 3";
		PassByReference pbr = new PassByReference();
		pbr.setString(str_1, str_3);
		pbr.setYourString(str_2);
		pbr.setMyString();
		System.out.println(str_1+" "+str_2+" "+str_3);
		pbr.printStrings();
	}

	public void setString(String orig, String newstr) {
		orig = newstr;
	}

	public void setYourString(String your) {
		your[1] = 'R';
		your[2] = 'O';
		your[3] = 'O';
		your[4] = 'T';
	}

	public void setMyString() {
		str = str3;
	}

	public void printStrings() {
		System.out.println(str+" "+str2+" "+str3);
	}
}

