# CPU
## CPU의 구성
<pre> CPU = CPU Core(Architecture) + CPU Peripherals(Controllers)</pre>

* ALU를 중심으로 **연산** 처리를 하는 CPU Core와 CPU Core에서 연산된 결과로 실제 하드웨어를 **제어** 하는 CPU Peripherals로 구성되어 있다.
* CPU Core와 CPU Peripherals는 서로 시스템 버스로 연결되어 있다.

## CPU의 동작 단계
1. Fetch
    * PC(Program Counter)가 가르키는 메모리 주소에 있는 해당 코드를 CPU Core로 가져오는 작업
2. Decode
    * CPU Core 내부에 있는 Latch에 잠시 저장된 Fetch로 가져온 코드를 어떤 명령어와 인자 값으로 구정되어 있는지 해석하는 작업
3. Execute
    * Decode 된 후 인자값들은 범용 레지스터에 저장되었다가 연산 처리를 하는 ALU에 의해 실행하는 작업

## 클럭
### 클럭이란?
* 어떤 작업이 순차적으로 진행되기 위해 언제 다음 작업으로 넘어가야 할지 시간을 측정할 때 사용되는 것

### 사이클
* 위에서 보면 하나의 CPU 명령어가 실행되기 위해 3단계의 작업, 즉 3클럭 정도가 소요된다고 예상할 수 있다.    
* 대부분의 CPU는 하나의 명령어가 실행되기까지 보통 3~4 클럭 정도가 소요된다.
* 이를 사이클이라 한다.

## Up to 16MIPS Throughput at 16MHz?
### MIPS이란?
* Million Instruction Per Second의 약자
* 1초당 몇 백만 개의 CPU 명령어가 수행되는지 CPU의 성능을 측정하는 단위
* 16MHz 스펙에서 최대 16백만개의 CPU 명령어가 수행될 수 있다.
* 1 주기(클럭)당 하나의 명령어가 수행될 정도의 빠른 CPU이며 이 CPU가 바로 AVR이다.

# AVR ATmega128이 범용적으로 사용되는 이유
* 초기 ZigBee 기반의 센서 네트워크 노드들이 대부분 ATmega128이었다.
* 아날로그 TV나 지하철, 버스 등에 사용되는 교통 카드 리더기 내에 ATmega128가 사용되었다.
* ATmega128 출시 때 ATmega128보다 내부 플래시 메모리 용량이 더 큰 MCU가 없었다.
* 작은 사이즈의 로봇 개발에 ATmega128이 자주 사용된다.
* 교육용으로 ATmega128을 범용적으로 사용했다.
