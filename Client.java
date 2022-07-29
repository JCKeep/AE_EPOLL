import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Client {
    public static void main(String[] args) throws Exception {
        Socket cli = new Socket();
        cli.connect(new InetSocketAddress("127.0.0.1", 8087));

        OutputStream out = cli.getOutputStream();
        out.write(0x2);
        Thread.sleep(1000);
        out.write("DISC".getBytes());
        System.out.printf("\033[32mBye bye!\033[0m\n");
        Thread.sleep(1000);
        cli.close();
    }
}
