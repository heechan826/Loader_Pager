# apager 실행 결과
<img width="650" alt="apager 결과" src="https://user-images.githubusercontent.com/47956399/116871382-20588c80-ac4f-11eb-92d4-f759735e4f80.PNG">

- **All-at-once loading 방법이기에 test 파일만 실행했을때와 running time의 차이가 크지 않았음**  
- **program header table에서 loadble한 엔트리(세그먼트)를 찾아서 매핑함으로써 elf파일을 로딩함**
- **겹치지 않는 주소에 새로운 stack 공간을 만들고 argc,argv,envp등을 넣어서 올바른 stack을 형성**
- **레지스터를 비우고 entry point로 점프 함으로써 test 프로그램 실행**
# dpager 실행 결과
<img width="650" alt="dpager 결과" src="https://user-images.githubusercontent.com/47956399/116871479-4716c300-ac4f-11eb-99b5-8fedcac0052b.PNG">

- **세그먼트들을 바로 로딩하여 매핑하는 대신, 자체적인 program header table에 로딩해둬서 보관하는 방식**
- **segmentation violation이 일어날때 직접 만들어둔 program header table에서 해당하는 주소의 segment를 로딩하게 구현하였다**
- **그러나 segmenation handler가 무한 호출이 되는 에러를 해결하지 못하여서 테스트 프로그램의 실행은 실패함.**
- **앞선 에러를 해결하지 못하여서, 이어지는 hpager의 구현까지는 도달하지 못하였다**
# test 파일만 실행 결과
<img width="450" alt="테스트 결과" src="https://user-images.githubusercontent.com/47956399/116871482-4aaa4a00-ac4f-11eb-8690-3c94e2522b39.PNG">

- **1부터 1,000,000,000까지 while문을 돌면서, 10,000,000의 배수일때만 숫자를 출력하는 테스트 프로그램**
- **편의상 10,000,000을 1로 치환하여 출력하였다**

# 참고 링크
#### 과제 출처
https://www.cs.utexas.edu/users/witchel/380L/lab/lab3.html  
#### 라이브러리 코드
https://elixir.bootlin.com/linux/v4.4/source/fs/binfmt_elf.c#L50
#### elf.h 헤더 파일
https://code.woboq.org/linux/include/elf.h.html  

