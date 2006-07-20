/* shows all envorinment properties
 */
import java.util.Properties;

public class ShowProperties {
	public static void main(String args[]) {
		Properties prop = System.getProperties();
		prop.list(System.out);
	}
}

