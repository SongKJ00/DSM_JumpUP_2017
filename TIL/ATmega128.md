### AVR ATmega128 Specificiation
* 메모리 : 128KB Flash, 4KB SRAM, 4KB EEPROM
* 입출력 : A~F port(각 8비트씩) + G port(5비트) = (8 * 6) + 5 = 53개의 입출력 핀
* 디버깅 : JTAG 인터페이스 지원
* 타이머/카운터 : 8비트 타이머/카운터 2개, 16비트 타이머/카운터 2개
* ADC : 8채널 10비트 ADC 내장
* 통신 : USART 2챼널, TWI 1챼널, SPI 1챼널
* 특이 사항 : Boot Loader를 통해 Self-Programming 지원

## Memory
* Program Memory, Data Memory
### Program Memory
* Flash Memory
* 컴파일한 HEX 코드(실행 프로그램)가 저장되는 공간
### Data Memory
* SRAM, EEPROM
* HEX 코드가 실행되기 위한 필요한 메모리들

## RISC
* RISC 프로세서는 사이즈가 고정적이기 때문에, Pre-Fetch 방식을 이용한다.
### Pre-Fetch
* CPU Instruction이 Fetch -> Decode -> Execute의 사이클에서 Execute 전에 Latch 공간이 비어있으면 미리 메모리에서 가져와 Fetch하는 작업
* CPU core의 연산 처리 능력을 조금 올릴 수 있음
