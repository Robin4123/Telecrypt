# 🛡️ Telecrypt

**Telecrypt** is a lightweight chat application that showcases the principles of **onion routing** — a technique designed to anonymize network traffic through layered encryption.

I WANT TO MAKE CLEAR THAT THIS WORKS ONLY ON WINDOWS MACHINES

## 🔍 What is Telecrypt?

Telecrypt was built as a demonstration tool to help understand how onion routing can be applied in real-time communication. It simulates a decentralized and privacy-focused chat system, where every message travels through a series of nodes, each adding or removing a layer of encryption.

### 🧅 How Onion Routing Works

- **Sending a Message (Client → Server):**  
  The client encrypts the message multiple times, once for each relay node. As the message traverses the network, each node removes one layer of encryption until it reaches the final destination in cleartext.

- **Receiving a Message (Server → Client):**  
  The message is encrypted at each hop with a new layer. Once it reaches the client, all encryption layers are peeled off in reverse order.

- **Privacy Benefit:**  
  Each node in the chain only knows its immediate predecessor and successor — no one node knows the full path or both ends of the communication.

## ⚙️ Tech Stack

- **Backend:**  
  Written entirely in **C++** using only basic standard libraries — no external dependencies.

- **Frontend:**  
  Built using **HTML**, **CSS**, and **JavaScript** to provide a clean and responsive user interface.

## 📄 Project Documentation

For a deeper dive into the architecture, routing protocol, and security model, check out the full project documentation:  
👉 [Read the full project document](https://docs.google.com/document/d/1EUIcCL7-NNXl5HXxe9FnYLt6977T8iHg2JSE1C2VJ0s/edit?usp=sharing)

---

Feel free to report issues, or suggest improvements!
