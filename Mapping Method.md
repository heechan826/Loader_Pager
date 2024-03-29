# Page Mapping Method
### 3가지 매핑 알고리즘
#### 1. Locality Based(Closest) Method: 매핑할 페이지에서 제일 가까운 페이지를 2nd page로 선택
#### 2. Locality Based(Farthest) Method: 매핑할 페이지에서 제일 먼 페이지를 2nd page로 선택
#### 3. Optimal Method: 참조될 순서를 실행을 통해 알아낸 후, 해당 순서에 맞게 2nd page를 선택   

### (참고) 기존 과제와 다른 구현 사항들
- **hybrid loading의 대상이 되는 page들을 대상으로 2,3-page mapping을 할것을 요구하였으나,   
    본 구현에서는 최대한 page들의 개수를 늘리기 위하여 demand loading의 대상인 page들에 대하여 2,3-page mapping을 하였다   
    (과제에서 중요하게 보는 부분이 결국 미리 page mapping을 하는 부분이라고 생각했어서, hpager가 아닌 dpager에서 구현을 해도 괜찮을 것이라고 판단하였다.).**
- **처음에는 1개의 page를 미리 mapping한 후, segmentation fault가 일어날때마다 segv handler의 호출을 통해 mapping을  
    하는 방식으로 구현을 해야하지만, 대상이 되는 page들의 개수를 늘리기 위하여 처음에 1개의 page를 mapping하는 부분은 제거하였다**
- **즉, dpager에서 segv handler를 통해서만 mapping이 일어나게 구현을 하였다.**
# 1-Page Mapping
<img width="500" alt="1개_최종" src="https://user-images.githubusercontent.com/47956399/123967087-f45c4c00-d9f0-11eb-9986-121d644a0059.PNG">

#### [구현 방식]
- **2-page, 3-page mapping을 사용하지 않고 page fault가 일어난 주소의 page만 mapping한다**
#### [실행 결과]
- **총 4개의 페이지가 존재할때 4번의 page fault가 일어남**
# 2-Page Mapping
### 1. Locality Based(Closest) Method
<img width="400" alt="2개_가까운거2" src="https://user-images.githubusercontent.com/47956399/123965430-67fd5980-d9ef-11eb-8715-cd07500bb621.PNG">
  
#### [구현 방식]
- **매핑할 페이지에서 제일 가까운 페이지를 2nd page로 고른다**
#### [실행 결과]
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어남**
### 2. Locality Based(Farthest) Method
<img width="400" alt="2개_먼거" src="https://user-images.githubusercontent.com/47956399/123953974-cec84600-d9e2-11eb-88cc-2d4645418a0e.PNG">

#### [구현 방식]
- **매핑할 페이지에서 제일 멀리 존재하는 페이지를 2nd page로 고른다**
#### [실행 결과]
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어남**
### 3. Optimal Method
<img width="400" alt="2개_최적" src="https://user-images.githubusercontent.com/47956399/123953985-d12aa000-d9e2-11eb-81b5-9c2fd8db6f8e.PNG">

#### [구현 방식]
- **참조될 순서를 미리 알아낸 후, 해당 순서에 맞게 2nd page를 고른다**
#### [실행 결과]
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어남**
# 3-Page Mapping 
### 1. Locality Based(Closest) Method
<img width="400" alt="3개_가까운거2" src="https://user-images.githubusercontent.com/47956399/123966055-ff62ac80-d9ef-11eb-9c9f-c15a121a27ae.PNG">
  
#### [실행 결과]
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어남**
### 2. Locality Based(Farthest) Method
<img width="400" alt="3개_먼거" src="https://user-images.githubusercontent.com/47956399/123954479-67f75c80-d9e3-11eb-91da-f6aef1580bba.PNG">

#### [실행 결과]
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어남**
### 3. Optimal Method
<img width="400" alt="3개_최적" src="https://user-images.githubusercontent.com/47956399/123954481-688ff300-d9e3-11eb-80f2-8ad964e3e6c4.PNG">

#### [실행 결과]
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어남**

# Page의 개수 확장
#### [Page를 쪼개는 방법]
<img width="450" alt="쪼갠거" src="https://user-images.githubusercontent.com/47956399/123954530-7776a580-d9e3-11eb-9712-0c1356e47d94.PNG">

- **총 4개의 page중 bss memory를 갖는 1개의 page만 제외하고, 나머지 3개의 page들을 각각 둘로 쪼개서 6개의 page로 생성하였다**
- **총 7개의 page가 존재하게 되었으나, 마지막 page를 loading하는 과정에서 Load 할 수 없는 메모리에 접근하는 현상이 발생하여 해결은 못하였다**
