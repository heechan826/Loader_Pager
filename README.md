# apager 실행 결과
<img width="650" alt="apager 결과" src="https://user-images.githubusercontent.com/47956399/116871382-20588c80-ac4f-11eb-92d4-f759735e4f80.PNG">

### 구현 방식  
- **program header table에서 loadble한 엔트리(세그먼트)를 찾아서 매핑함으로써 elf파일을 로딩함**
- **겹치지 않는 주소에 새로운 stack 공간을 만들고 argc,argv,envp등을 넣어서 올바른 stack을 형성**
- **레지스터를 비우고 entry point로 점프 함으로써 test 프로그램 실행**
### 실행 시간
- **All-at-once loading 방법이기에 test 파일만 실행했을때와 running time의 차이가 크지 않았음**  
# dpager 실행 결과
<img width="650" alt="dpager" src="https://user-images.githubusercontent.com/47956399/119264938-f2e47a80-bc1f-11eb-9587-9b858ea792ee.PNG">

### 구현 방식
- **세그먼트들을 바로 로딩하는 대신, 처음엔 1개의 세그먼트만 매핑한다**
- **이후 segmentation violation이 일어날때, violation이 일어난 주소에 해당하는 세그먼트를 매핑시켜준다**
- **기존의 구현 방식은 처음 세그먼트 로딩시, 세그먼트들을 program header table에 로딩해둬서 보관하는 방식이었다**
- **그러나 기존의 방식의 구현으로는 segmentation handler가 무한으로 호출되는 에러를 해결하지 못하였다.**
- **이에 대한 대안으로 header table에 보관하는 방법 대신 세그먼트 매핑이 필요할때 다시 file descriptor로 접근하여 매핑하는 방식으로 구현하였다.**
- **해당 방법으로는 에러가 더 일어나지 않았다.**
### 실행 시간
- **실행 시간은 apager 방법과 큰 차이가 나지 않았다.** 
- **test 프로그램이 한창 실행인 도중에 segmentation fault가 일어나지 않고, 제대로 시작하기 전에 segmentation fault가 일어난다.** 
- **사실상 test 프로그램의 제대로 된 실행전에 이미 다 세그먼트들이 mapping이 완료된 상태이기에 실행 시간이 차이가 나지 않는것 같다.**

# hpager 실행 결과
<img width="650" alt="hpager" src="https://user-images.githubusercontent.com/47956399/119264989-1b6c7480-bc20-11eb-9080-6f63e55adb1b.PNG">

### 구현 방식
- **apager 방식과 비슷하게 loadble한 세그먼트들을 찾아서 한번에 로딩한다**
- **하지만 bss memory 부분은 로딩시 매핑을 시키지 않는다**
- **추후 bss memory 부재로 인해 segmentation fault가 나면, 해당 세그먼트에 해당하는 bss memory 부분만 segmentation handler로 매핑시킨다**
- **본래 과제에서는 handler를 통한 첫 매핑시 prediction 알고리즘으로 다른 세그먼트도 미리 매핑하는것을 요구하였으나, 사용한 test 코드들에서는 segmentation fault가 일어난 세그먼트가 1개뿐이었기에 미리 매핑하는 방식은 구현되지 않았다.**
### 실행 시간
- **실행 시간은 apager, dpager, hpager 모두가 비슷하다**
- **dpager에서와 마찬가지로, test 프로그램이 한창 실행인 도중에 segmenation fault가 일어나지 않고 초반에 segmentation fault가 다 일어나기에 비슷한 실행시간을 갖는듯 하다**
# 사용한 test 코드
<img width="450" alt="테스트 결과" src="https://user-images.githubusercontent.com/47956399/116871482-4aaa4a00-ac4f-11eb-8690-3c94e2522b39.PNG">  

- **test1 코드를 이용하여 테스트하였다**
- **1부터 1,000,000,000까지 while문을 돌면서, 10,000,000의 배수일때만 숫자를 출력하는 테스트 프로그램**
- **편의상 10,000,000을 1로 치환하여 출력하였다**

# Mapping Method의 구현
## 1-Page Mapping
<img width="600" alt="dpager실행창" src="https://user-images.githubusercontent.com/47956399/123953429-2ca85e00-d9e2-11eb-9a7d-c0874467d6c0.PNG">  

- **2page, 3page mapping을 사용하지 않고 page fault가 일어난 주소의 page만 mapping한다**
- **총 4개의 페이지가 존재할때 4번의 page fault가 일어난다**
## 2-Page Mapping
### 1. Locality Based(Closest) Method
<img width="450" alt="2개_가까운거" src="https://user-images.githubusercontent.com/47956399/123953959-cbcd5580-d9e2-11eb-8c67-3f3712acb484.PNG">
  
- **매핑할 페이지에서 제일 가까운 페이지를 2nd page로 고른다**
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
### 2. Locality Based(Farthest) Method
<img width="450" alt="2개_먼거" src="https://user-images.githubusercontent.com/47956399/123953974-cec84600-d9e2-11eb-88cc-2d4645418a0e.PNG">
  
- **매핑할 페이지에서 제일 멀리 존재하는 페이지를 2nd page로 고른다**
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
### 3. Optimal Method
<img width="450" alt="2개_최적" src="https://user-images.githubusercontent.com/47956399/123953985-d12aa000-d9e2-11eb-81b5-9c2fd8db6f8e.PNG">
  
- **참조될 순서를 미리 알아낸 후, 해당 순서에 맞게 2nd page를 고른다**
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
## 3-Page Mapping 
### 1. Locality Based(Closest) Method
<img width="450" alt="3개_가까운거" src="https://user-images.githubusercontent.com/47956399/123954478-67f75c80-d9e3-11eb-91a6-55bbdc4d16b7.PNG">
  
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
### 2. Locality Based(Farthest) Method
<img width="450" alt="3개_먼거" src="https://user-images.githubusercontent.com/47956399/123954479-67f75c80-d9e3-11eb-91da-f6aef1580bba.PNG">

- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
### 3. Optimal Method
<img width="450" alt="3개_최적" src="https://user-images.githubusercontent.com/47956399/123954481-688ff300-d9e3-11eb-80f2-8ad964e3e6c4.PNG">

- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**


# 참고 링크
#### 과제 출처
https://www.cs.utexas.edu/users/witchel/380L/lab/lab3.html  
#### 라이브러리 코드
https://elixir.bootlin.com/linux/v4.4/source/fs/binfmt_elf.c#L50
#### elf.h 헤더 파일
https://code.woboq.org/linux/include/elf.h.html  

