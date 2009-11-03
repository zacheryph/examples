/* Sekure: simple file encryption
 *
 * Copyright (C) 2005 Zachery Hostens <zacheryph@gmail.com>
 *
 * License GPL
 */
import java.io.*;
import java.security.*;
import java.security.spec.*;
import java.util.Arrays;
import javax.crypto.*;
import javax.crypto.spec.*;
import sun.misc.BASE64Encoder;
import sun.misc.BASE64Decoder;

public class Sekure
{
	private static int iterations = 19;
	private static byte[] salt = {
		(byte) 0xFE, (byte) 0xA2, (byte) 0x77, (byte) 0x3C,
		(byte) 0x03, (byte) 0xA8, (byte) 0x82, (byte) 0xE3
	};

	public static void main (String argv[]) {
		int mode = 0;

		if (argv.length == 0)
			printUsage();

		try {
			File f = new File (argv[0]);
			
			if (!f.exists() || !f.canRead()) {
				System.out.printf("file does not exist: %s\n", argv[0]);
				System.exit (0);
			}

			if (Sekure.isSekureFile(f))
				mode = Cipher.DECRYPT_MODE;
			else
				mode = Cipher.ENCRYPT_MODE;

			/* read/create passwd. fix this later! */
			String pass = Sekure.readPassword();

			PBEKeySpec spec = new PBEKeySpec(pass.toCharArray());
			AlgorithmParameterSpec param =
					new PBEParameterSpec(salt, iterations);
			SecretKey key = SecretKeyFactory.getInstance("PBEWithMD5AndDES").generateSecret(spec);
			Cipher cip = Cipher.getInstance(key.getAlgorithm());
			cip.init(mode, key, param);
			if (mode == Cipher.ENCRYPT_MODE) {
				InputStream in = new CipherInputStream(new FileInputStream(f), cip);
				OutputStream out = new FileOutputStream(f.getName()+".sek");
				BASE64Encoder benc = new BASE64Encoder();

				out.write("sek!".getBytes());
				out.write(benc.encode(Sekure.getPasswordHash(pass)).getBytes());
				out.write("\n".getBytes());

				benc.encodeBuffer(in, out);

				in.close();
				out.close();
				File newFile = new File(f.getName()+".sek");
				newFile.renameTo(f);
			}
			else {
				String newFileName;
				if (f.getName().substring(f.getName().length() - 4).equals(".sek"))
					newFileName = f.getName().substring(0, f.getName().length()-4);
				else
					newFileName = f.getName()+".dec";

				//InputStream in = new FileInputStream(f);
				BufferedReader in = new BufferedReader(new FileReader(f));
				OutputStream out = 
					new CipherOutputStream(new FileOutputStream(newFileName), cip);
				BASE64Decoder bdec = new BASE64Decoder();

				String line;
				
				// check password
				line = in.readLine().substring(4);
				byte[] encpass = bdec.decodeBuffer(line);
				byte[] entpass = Sekure.getPasswordHash(pass);
				if (! Arrays.equals(encpass, entpass)) {
					System.out.println("sekure: incorrect password");
					File newFile = new File(newFileName);
					newFile.delete();
					System.exit(0);
				}
				
				while ((line = in.readLine()) != null)
					out.write(bdec.decodeBuffer(line));

				in.close();
				out.close();
				File newFile = new File(newFileName);
				newFile.renameTo(f);
			}
		}
		catch (Exception ex) {
			System.out.println(ex);
		}
	}

	public static boolean isSekureFile (File f) {
		try {
			FileReader read = new FileReader (f);
			char check[] = new char[4];
		
			read.read(check, 0, 4);
			read.close();

			String tmp = new String(check);

			if (tmp.equals("sek!"))
				return true;
			return false;
		}
		catch (Exception ex) {
			System.err.println(ex);
		}
		return false;
	}

	public static String readPassword () {
		PasswordField passfield = new PasswordField();
		String pass = null;
		try {
			pass = passfield.getPassword("password: ");	
		}
		catch (Exception ex) {
			System.err.println(ex);
		}
		return pass;
	}

	public static byte[] getPasswordHash (String pass) {
		try {
			MessageDigest md = MessageDigest.getInstance("MD5");
			return md.digest(pass.getBytes());
		}
		catch (Exception ex) {
			System.out.println(ex);
		}
		return null;
	}
			
	public static void printUsage () {
		System.out.printf("usage: sekure <file>\n");
		System.out.printf("  sekure will automatically notice if this file\n");
		System.out.printf("  is to be encrypted or decrypted and inform you\n");
		System.exit (0);
	}
}
