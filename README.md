# Color-Attenuation-Prior-Dehazing
C++ codes for the paper "A Fast Single Image Haze Removal Algorithm using Color Attenuation Prior"

## OpenCV sort usage:
```c++=
#define CV_SORT_EVERY_ROW		0
#define CV_SORT_ASCENDING		0
#define CV_SORT_EVERY_COLUMN	1
#define CV_SORT_DESCENDING		16

void demoSort()
{
	int testArrLen = 3;
	Mat_<int> testArr = cv::Mat::zeros(3, testArrLen, CV_64F);
	testArr(0, 0) = 100;
	testArr(0, 1) = 3;
	testArr(0, 2) = 5;
	testArr(1, 0) = 15;
	testArr(1, 1) = 87;
	testArr(1, 2) = 99;
	testArr(2, 0) = 33;
	testArr(2, 1) = 55;
	testArr(2, 2) = 44;
	Mat_<int> sortArr,sortIdxArr;
	cv::sortIdx(testArr, sortIdxArr, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	cv::sort(testArr, sortArr, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	cout << "testArr = \n" << testArr << endl;
	cout << "sortArr = \n" << sortArr << endl;
	cout << "sortIdx = \n" << sortIdxArr << endl;
}
```

## Abstract

In this paper, we propose a simple but powerful color attenuation prior for haze removal from a single input hazy image. By creating a linear model for modeling the scene depth of the hazy image under this novel prior and learning the parameters of the model with a supervised learning, the depth information can be well recovered.

## Introduction 

Outdoor images taken in bad weather (e.g. foggy or haze) usually lose contrast and fidelity, resulting rom the fact that light is absorbed and scattered by the turbid medium such as particles and water droplets in the atmosphere during the process of propagation.

## Atmospheric Scattering Model

𝐈(𝑥)=𝑱(𝑥)t(𝑥)+𝑨(1−t(𝑥))

t(𝑥)= 𝑒^(−𝛽𝑑(𝑥) )

𝐈(𝑥):haze image // input image
𝑱(𝑥):dehaze image // output image
𝑨: global atmospheric light
t(𝑥): medium transmission
𝛽: scattering coefficient
𝑑(𝑥): scene depth
![](https://i.imgur.com/Y1AGqDN.png)


## Color Attenuation Prior

![](https://i.imgur.com/9lMOjbp.jpg)

a. 一張有霧的圖片
b. 一張圖中霧霾比較密集的程度
c. 沒那麼密集但還是明顯的部分
d. 沒有霧霾的部分

當我們將圖片轉成HSV形式後，我們可以發現當value與saturation的difference越大，霧霾的程度越嚴重。

![](https://i.imgur.com/iBQSBgk.png)

a. 近景
b. 中景
c. 遠景
這幾張圖分別可以表示原圖與景深的關係


## Scene Depth Restoration

而為了方便計算，我們將d(x)，也就是scene depth model成這個linear equation。根據Gaussian distribution可以得到以下這個式子。
接下來這篇paper利用了500張沒有霧霾的圖片來當作資料集，並以人工的方式對圖片生成了人工的霧霾。接著利用supervised learning 來找到這個式子的參數們。

![](https://i.imgur.com/xCGiPPg.png)

![](https://i.imgur.com/aGFdsU5.jpg)


這個式子是對整張圖片去做erosion

## Guided Filter
![](https://i.imgur.com/kp6Yg8h.png)
導向濾波的名字由來，在於其演算法中，對於一張輸入圖p，我們還額外需要提供另一張引導圖 I，才有辦法做濾波(當然引導圖也可以是同一張輸入圖)，其架構示意圖如下：
注意到圖的下半部分，導向濾波首先假設輸出圖 q 與引導圖 I 之間有局部線性的關係，而輸出圖 q 同時也等同輸入圖 p 去除雜訊 n 後的結果。因此我們可以得到一組聯立方程式，考慮無約束的影像還原方法，我們把雜訊與其他變數分別置於等號兩邊，目標最小化 n² 並施加正則化 (Regularization)，得到 a 與 b 在window k 中的最小平方式
而其解為ak與bk

其中，aₖ的分子為 I 與 p 對應座標 window的共變異數 (covariance)，分母為 I 影像中各座標 windows 的變異數。

### Pseudo code
![](https://i.imgur.com/9yvErQm.png)

## Scene Radiance Recovery

In haze image, the atmospheric light is similar to the sky and we pick the top 0.1 percent brightest pixels in the depth map


𝐉(𝐱)=(𝐈(𝐱)−𝑨)/(min{max⁡(t(𝐱),0.1),0.9})+𝑨

## Experimental Results

![](https://i.imgur.com/ROrPg3e.jpg)

![](https://i.imgur.com/wlDKQ7F.jpg)

![](https://i.imgur.com/QqmmvB1.jpg)

## References

[1] E. J. McCartney, Optics of the Atmosphere: Scattering by Molecules and Particles. New York, NY, USA: Wiley, 1976.
[2] Q. Zhu, J. Mai, and L. Shao, “A fast single image haze removal algorithm using color attenuation prior,” IEEE transactions on image processing, vol. 24, no. 11, pp. 3522–3533, 2015.
[3] K. He, J. Sun, and X. Tang, “Guided image filtering,” IEEE transactions on pattern analysis and machine intelligence, vol. 35, no. 6, pp. 1397–1409, 2012.
[4]導向濾波的原理以及其應用. Guided Filter | by 黃琮耀 | Medium
