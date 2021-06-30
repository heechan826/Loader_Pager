# 1-Page Mapping
<img width="600" alt="dpager실행창" src="https://user-images.githubusercontent.com/47956399/123953429-2ca85e00-d9e2-11eb-9a7d-c0874467d6c0.PNG">  

- **2page, 3page mapping을 사용하지 않고 page fault가 일어난 주소의 page만 mapping한다**
- **총 4개의 페이지가 존재할때 4번의 page fault가 일어난다**
# 2-Page Mapping
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
# 3-Page Mapping 
### 1. Locality Based(Closest) Method
<img width="450" alt="3개_가까운거" src="https://user-images.githubusercontent.com/47956399/123954478-67f75c80-d9e3-11eb-91a6-55bbdc4d16b7.PNG">
  
- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
### 2. Locality Based(Farthest) Method
<img width="450" alt="3개_먼거" src="https://user-images.githubusercontent.com/47956399/123954479-67f75c80-d9e3-11eb-91da-f6aef1580bba.PNG">

- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
### 3. Optimal Method
<img width="450" alt="3개_최적" src="https://user-images.githubusercontent.com/47956399/123954481-688ff300-d9e3-11eb-80f2-8ad964e3e6c4.PNG">

- **총 4개의 페이지가 존재할때 2번의 page fault가 일어난다**
