import java.util.LinkedList;
import java.util.concurrent.Semaphore;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;

class Buffer {
    private LinkedList<Integer> buffer = new LinkedList<>();
    private int capacity = 5; // Max buffer size
    private Semaphore mutex = new Semaphore(1); // Controls access to buffer
    private Semaphore empty = new Semaphore(5); // Counts empty slots
    private Semaphore full = new Semaphore(0); // Counts filled slots

    public void produce(int item) throws InterruptedException {
        empty.acquire(); // Wait if buffer is full
        mutex.acquire(); // Lock buffer
        System.out.println("[Producer] Waiting to produce item " + item);
        buffer.add(item);
        System.out.println("[Producer] Produced item " + item + ", Buffer: " + buffer);
        mutex.release(); // Unlock buffer
        full.release(); // Signal an item is added
    }

    public int consume(int pid) throws InterruptedException {
        full.acquire(); // Wait if buffer is empty
        mutex.acquire(); // Lock buffer
        System.out.println("[Consumer " + pid + "] Waiting to consume...");
        int item = buffer.removeFirst();
        System.out.println("[Consumer " + pid + "] Consumed item " + item + ", Buffer: " + buffer);
        mutex.release(); // Unlock buffer
        empty.release(); // Signal a slot is free
        return item;
    }
}

class Producer extends Thread {
    private Buffer buffer;
    private int pid, burstTime;
    public Producer(int pid, int burstTime, Buffer buffer) {
        this.pid = pid;
        this.burstTime = burstTime;
        this.buffer = buffer;
    }
    public void run() {
        for (int i = 0; i < burstTime; i++) {
            try {
                buffer.produce(pid * 10 + i); // Unique item based on pid
                Thread.sleep(1000); // Simulate work (1 second per item)
            } catch (InterruptedException e) {
                System.out.println("[Producer " + pid + "] Interrupted");
            }
        }
        System.out.println("[Producer " + pid + "] Finished");
    }
}

class Consumer extends Thread {
    private Buffer buffer;
    private int pid, burstTime;
    public Consumer(int pid, int burstTime, Buffer buffer) {
        this.pid = pid;
        this.burstTime = burstTime;
        this.buffer = buffer;
    }
    public void run() {
        for (int i = 0; i < burstTime; i++) {
            try {
                buffer.consume(pid);
                Thread.sleep(1500); // Simulate work (1.5 seconds per item)
            } catch (InterruptedException e) {
                System.out.println("[Consumer " + pid + "] Interrupted");
            }
        }
        System.out.println("[Consumer " + pid + "] Finished");
    }
}

public class ProducerConsumer {
    public static void main(String[] args) throws Exception {
        Buffer buffer = new Buffer();
        ArrayList<Thread> threads = new ArrayList<>();

        // Read processes.txt, skipping the header
        BufferedReader reader = new BufferedReader(new FileReader("processes.txt"));
        String line;
        boolean firstLine = true;
        boolean firstProcess = true;
        while ((line = reader.readLine()) != null) {
            if (firstLine) {
                firstLine = false; // Skip header
                continue;
            }
            String[] parts = line.split("\\s+"); // Split by whitespace
            int pid = Integer.parseInt(parts[0]); // PID
            int burstTime = Integer.parseInt(parts[2]); // Burst_Time (ignore Arrival_Time and Priority)

            if (firstProcess) {
                threads.add(new Producer(pid, burstTime, buffer));
                firstProcess = false;
            } else {
                threads.add(new Consumer(pid, burstTime, buffer));
            }
        }
        reader.close();

        // Start all threads
        for (Thread t : threads) {
            t.start();
        }
        // Wait for all to finish
        for (Thread t : threads) {
            t.join();
        }
        System.out.println("Simulation complete!");
    }
}