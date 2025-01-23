# OpenACR

> A high-performance meta-programming language for building scalable systems with code generation.

## Overview
OpenACR is a comprehensive toolkit for building high-performance systems through code generation and meta-programming. It enables developers to create powerful, extensible applications by treating code as data and automating the generation of optimized system components.

## Why OpenACR?

- **Write Less, Do More**: Generate optimized C++ code automatically from simple schema definitions
- **Scale with Confidence**: Used in production for 15+ years in high-stakes financial systems
- **Future-Proof Architecture**: Treat code as data to evolve your system organically
- **Developer-First Experience**: Human-readable configuration format that plays well with version control

### Proven Performance
OpenACR has been maintained, improved, and used in production for over 15 years. It was originally created for (and matured at) the New York Stock Exchange<!-- *Well technically, it was originally created at Algo Inc, which was then acquired by NYSE to develop pillar -->, where all aspects of a system are critically important. As such, it was meticulously designed to maintain and outperform regulated standards of speed, availability, and throughput, while maintaining the flexibility of its design principles.

Now, it powers...

📈 ...ultra-low-latency trading systems processing hundreds of billions of messages daily, such as [Pillar](https://www.nyse.com/pillar)

🌀 ...high-throughput data streaming platforms running at 1tbps such as [AlgoX2](https://algox2.com/)

👀 ...high-availability real-time monitors across infrastructures of thousands of servers

🤖 ...complex CI/CD pipelines and infrastructure orchestration systems

🔄 ...itself! 95% of OpenACR's source code is generated using its own code generator.

and more!

## Key Features

### 📝 Schema-First Development
- Define your system architecture using ssimfiles - a lightweight, relational format
- Clean separation of data and logic
- Version-control friendly
- Perfect for rapid prototyping

### 🚀 Automated Code Generation
- Optimized C++ data structures and validation logic
- Efficient access paths and indexes
- Rich data structure support (hash tables, binary heaps, trees)
- Language-agnostic schema format (more languages planned)

### 🔧 Flexible Integration
- Enhance existing projects incrementally
- Pick and choose what to manage with OpenACR
- Comprehensive standard library
- Use the schema system even without C++
  



## Installation
1. Clone the repository
```bash
git clone https://github.com/alexeilebedev/openacr.git
cd openacr
``` 
2. Install dependencies:
```bash
# Ubuntu/Debian
sudo apt install -y mariadb-server mariadb-client libmariadb-dev libssl-dev liblz4-dev cppcheck

# CentOS
sudo yum install -y mariadb mariadb-devel mariadb-server
```
3. Build
```bash
bin/ai
```

## Contributing

Contributions are welcome! If you'd like to help improve OpenACR, feel free to:
- 🐛 Report bugs and issues
- 💡 Suggest new features
- 📝 Improve documentation
- 🔧 Submit pull requests

Please review the Contributing Guidelines before getting started.

## 💬 Community
Join the new [Discord](https://discord.gg/ZmbsvPzUAX)


## License

OpenACR is licensed under the GNU General Public License (GPL). See [LICENSE](https://github.com/alexeilebedev/openacr/blob/master/LICENSE) for details.
