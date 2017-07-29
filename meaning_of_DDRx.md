흔히 AVR 프로세서를 사용할 때 포트의 입출력을 설정하고 싶다면 이런 코드를 사용한다.

### DDRB = 0xFF;

여기서 우리가 사용하고 있는 DDRB라는 것을 그냥 입출력을 설정하는 레지스터라는 것일 뿐이라고 알고 있을 것이다.<br> 단순히 레지스터의 한 비트를 셋하면 그 핀은 출력으로 설정되고, 클리어하면 입력으로 설정되는 것은 충분히 알 것이다.

### 하지만, 여기서 0xFF라는 값을 대입해주는 것으로 보아서 DDRB는 소스 코드에서 그 문자 'DDRB' 자체로 무엇을 뜻하는 것일까?

<br>그래서 한 [블로그](http://liketech.tistory.com/entry/DDRB0xFF-%EC%9D%98-%EC%9D%98%EB%AF%B8)를 찾아보았다.<br> 다음 내용들은 이 블로그를 참고로 내가 이해한 내용들을 정리해 놓은 것이다.(ATmega128을 예로 설명하겠다)<br><br>

먼저 **DDRB**는 **<iom128.h>** 이라는 헤더파일에 정의된 매크로 상수이다.<br>**<iom128.h>** 내용을 들여다 보면 이러한 내용이 있다.<br><pre>#define DDRB _SFR_IO8(0x17)</pre>

여기서 **SFR**이라는 것은 무엇일까?<br><br>**SFR**은 **Special Function Register**의 약자로 말 그대로 특별한 기능을 가진 레지스터이다.<br> 특별한 기능을 가졌다면 우리가 컴퓨터 구조에서 배웠던 register와는 조금 다른걸까?<br><br> 우리가 흔히 알고 있던 그 register들은 **General Purpose Register**라고 불리는 것으로 operand나 연산의 결과를 저장하는 저장장치이다.<br>

여기서, **SFR**은 **General Purpose Register**와 다르게 주로 MCU 분야에서 사용되는 용어이다.<br> 즉, 특별한 기능인 I/O, 타이머, 인터럽트, UART, SPI, TWI 등을 제어할 수 있는 레지스터를 **SFR**이라 한다.<br><br><br><br>

### 그럼 _SFR_IO8은 무엇이라고 define되어 있을까?

이 내용을 찾기 위해선 **<sfr_defs.h>** 이라는 헤더 파일을 찾아보아야 했다.<br>

<pre>#define    _SFR_IO8(io_addr)    _MMIO_BYTE((io_addr) + __SFR_OFFSET)</pre>
**<sfr_defs.h>** 에 저장된 이 내용을 통해 **io_addr**에 **__SFR_OFFSET**이라는 것을 더해 무엇인가 주소를 설정하는 것이라는 것을 추측해볼 수 있었다.<br><br>
그리고, 이번에는 **__SFR_OFFSET**이 뭔지 찾아보아야 했다.<br>
**__SFR_OFFSET**은 **_SFR_IO8**과 같이 <sfr_defs.h>에 정의되어 있었다.<br><br>

<pre>#ifndef __SFR_OFFSET
#&nbsp;&nbsp;if __AVR_ARCH__ >= 100
#&nbsp;&nbsp;&nbsp;&nbsp;define __SFR_OFFSET 0x00
#&nbsp;&nbsp;else
#&nbsp;&nbsp;&nbsp;&nbsp;define __SFR_OFFSET 0x20
#&nbsp;&nbsp;endif
#endif</pre>

<br>**_SFR_OFFSET**에 대한 내용은 위와 같이 나와 있다.<br><br> 다만, 여기서 의문이 드는 것은 저 \****AVR_ARCH****라는 것이다.<br> 저 if문은 무엇을 의미하길래 **\__SFR_OFFSET**의 값이 두 개로 나뉘어지는 것일까?<br><br> 이 의문을 해결하기 위해 한 [사이트](http://www.avrfreaks.net/forum/exclude-ioh)를 방문해 보았다.<br><br>

이 사이트의 내용을 정리하자면, \****AVR_ARCH****라는 것은 어느 헤더파일에도 정의되어 있지 않다고 한다.<br> 그렇지만, 여러분들이 컴파일하는 순간 미리 지정해 놓았던 MCU 종류에 따라 컴파일러가 *\**AVR_ARCH*\_\** 값을 정의한다고 한다.<br><br> 범용적으로 사용하는 **ATmega128**같은 경우에는 [ATmel 홈페이지](http://www.atmel.com/webdoc/AVRLibcReferenceManual/using_tools_1using_avr_gcc_mach_opt.html)에 따르면 **Architecture**, **MCU name**, **Macro**가 각각 다음과 같다.<pre>avr5/avr51[3] ATmega128 **AVR_ATmega128**</pre> 여기서 **ATmega128**의 **Architecture**인 **avr5**와 **avr51**에 대해 살펴보자.<br> 다시 한번 [ATmel 홈페이지](http://www.atmel.com/webdoc/AVRLibcReferenceManual/using_tools_1using_avr_gcc_mach_opt.html)를 살펴보면 **Architecture**에 따른 **Macro**가 나와있다.<br><br> 여기서 **Macro**는 위에서 잠깐 설명했던 **Macro**와는 조금 다른 것으로, \****AVR_ARCH****의 값을 정의하는 부분이다.<br>

<pre>avr5&nbsp;&nbsp;&nbsp;&nbsp;__AVR_ARCH__=5 __AVR_MEGA__[5] __AVR_ENHANCED__[5] __AVR_HAVE_JMP_CALL__[4] __AVR_HAVE_MOVW__[1] __AVR_HAVE_LPMX__[1] __AVR_HAVE_MUL__[1] __AVR_2_BYTE_PC__[2]</pre>
<pre>avr51&nbsp;&nbsp;&nbsp;&nbsp;__AVR_ARCH__=51 __AVR_MEGA__[5] __AVR_ENHANCED__[5] __AVR_HAVE_JMP_CALL__[4] __AVR_HAVE_MOVW__[1] __AVR_HAVE_LPMX__[1] __AVR_HAVE_MUL__[1] __AVR_HAVE_RAMPZ__[4] __AVR_HAVE_ELPM__[4] __AVR_HAVE_ELPMX__[4] __AVR_2_BYTE_PC__[2]</pre>

이 자료를 통해 **MCU name**에 따른 **Architecture**가 **avr5**일 때는 \****AVR_ARCH****값이 **5**, **avr51**일 때는 \****AVR_ARCH****값이 **51**라는 것을 알 수 있다.<br><br>

각 **Architecture**의 특징을 살펴보자면 **avr5**의 **MCU**들은 향상된 CPU core와 16KB에서 64KB까지의 ROM을 가지고 있고**avr51**의 **MCU**들은 향상된 CPU core와 128KB의 ROM을 가지고 있다.<br><br>

결론적으로는 **ATmega128**은 \****AVR_ARCH****값이 100보다 작으므로 **\__SFR_OFFSET**값은 0x20이다.(위쪽에 있던\__SFR_OFFSET 코드 참고)<br><br>

이 기나긴 글을 읽느라 이전에 있던<pre>#define \__SFR_IO8(io_addr) _MMIO_BYTE((io_addr) + \__SFR_OFFSET)</pre> 은 까먹었겠지만 다시 한번 참고하자.<br><br>

우리는 어쨋든 **ATmega128** 사용 시 **\__SFR_OFFSET** 값이 0x20이라는 것을 알게 되었다.(다른 MCU들도 거의 0x20이다.)<br> 그렇다면 **_MMIO_BYTE**는 무엇일까? 다시 **<sfr_defs.h>** 를 뒤져보자.( 아직 끝나지 않았다:) )<br><br>

<pre>#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t*)(mem_addr))</pre> 이렇게 나와있다.<br> 즉, 포인터로 특정 주소 **mem_addr**에 접근한다는 뜻이다.<br><br><br><br>

### 좋다. 기승전결 중 '기승전'이 끝났다. 이제 '결'을 내보자.

우리는 **DDRB = 0xFF**라는 코드를 통해 포트의 입출력 설정을 했었다.<br><br>**DDRB**가 정의된 헤더파일 **<iom128.h>** 에 들어가 보니<pre>#define DDRB _SFR_IO8(0x17)</pre> 로 **DDRB**가 정의되어 있었다.<br><br>

**_SFR_IO8**은 헤더파일 **<sfr_defs.h>** 에 다음과 같이 정의되어 있었다.<pre>#define _SFR_IO8(io_addr) _MMIO_BYTE((io_addr) + \__SFR_OFFSET)</pre><br>

**\__SFR_OFFSET**은 MCU와 컴파일러에 의해 컴파일 시 **0x20**이란 값으로 세팅이 되었고<br>**_MMIO_BYTE**는 **<sfr_defs.h>** 에 다음과 같이 정의되어 있었다.<pre>#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t *)mem_addr)</pre><br><br>

따라서!
=======

우리가 **DDRB=0xFF**라고 코드를 작성한다는 것은 다음과 같다.<pre>\(*(volatile uint8_t *)(0x17+0x20)) = 0xFF</pre> 즉, 메모리 주소 **0x37** 번지에 있는 레지스터의 값을 **0xFF** , 8개 비트를 **1**로 설정한다는 것을 의미한다.<br><br>

실제로 [ATmega128 Datasheet](http://www.atmel.com/images/doc2467.pdf)을 찾아보면 **DDRB** 레지스터는 **0x37**번지에 위치한 것을 알 수 있다.<br><br>

결국, 코드 내에서의 **DDRB = 0xFF**는 **DDRB**의 주소인 **0x37**번지에 **0xFF**라는 값을 넣는 **함수 매크로**를 실행하는 것이었다.
