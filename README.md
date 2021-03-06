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

π(π₯)=π±(π₯)t(π₯)+π¨(1βt(π₯))

t(π₯)= π^(βπ½π(π₯) )

π(π₯):haze image // input image
π±(π₯):dehaze image // output image
π¨: global atmospheric light
t(π₯): medium transmission
π½: scattering coefficient
π(π₯): scene depth
![](https://i.imgur.com/Y1AGqDN.png)


## Color Attenuation Prior

![](https://i.imgur.com/9lMOjbp.jpg)

a. δΈεΌ΅ζι§ηεη
b. δΈεΌ΅εδΈ­ι§ιΎζ―θΌε―ιηη¨εΊ¦
c. ζ²ι£ιΊΌε―ιδ½ιζ―ζι‘―ηι¨ε
d. ζ²ζι§ιΎηι¨ε

ηΆζεε°εηθ½ζHSVε½’εΌεΎοΌζεε―δ»₯ηΌηΎηΆvalueθsaturationηdifferenceθΆε€§οΌι§ιΎηη¨εΊ¦θΆε΄ιγ

![](https://i.imgur.com/iBQSBgk.png)

a. θΏζ―
b. δΈ­ζ―
c. ι ζ―
ιεΉΎεΌ΅εεε₯ε―δ»₯θ‘¨η€Ίεεθζ―ζ·±ηιδΏ


## Scene Depth Restoration

θηΊδΊζΉδΎΏθ¨η?οΌζεε°d(x)οΌδΉε°±ζ―scene depth modelζιεlinear equationγζ ΉζGaussian distributionε―δ»₯εΎε°δ»₯δΈιεεΌε­γ
ζ₯δΈδΎιη―paperε©η¨δΊ500εΌ΅ζ²ζι§ιΎηεηδΎηΆδ½θ³ζιοΌδΈ¦δ»₯δΊΊε·₯ηζΉεΌε°εηηζδΊδΊΊε·₯ηι§ιΎγζ₯θε©η¨supervised learning δΎζΎε°ιεεΌε­ηεζΈεγ

![](https://i.imgur.com/xCGiPPg.png)

![](https://i.imgur.com/aGFdsU5.jpg)


ιεεΌε­ζ―ε°ζ΄εΌ΅εηε»εerosion

## Guided Filter
![](https://i.imgur.com/kp6Yg8h.png)
ε°εζΏΎζ³’ηεε­η±δΎοΌε¨ζΌεΆζΌη?ζ³δΈ­οΌε°ζΌδΈεΌ΅θΌΈε₯εpοΌζειι‘ε€ιθ¦ζδΎε¦δΈεΌ΅εΌε°ε IοΌζζθΎ¦ζ³εζΏΎζ³’(ηΆηΆεΌε°εδΉε―δ»₯ζ―εδΈεΌ΅θΌΈε₯ε)οΌεΆζΆζ§η€Ίζεε¦δΈοΌ
ζ³¨ζε°εηδΈει¨εοΌε°εζΏΎζ³’ι¦εεθ¨­θΌΈεΊε q θεΌε°ε I δΉιζε±ι¨η·ζ§ηιδΏοΌθθΌΈεΊε q εζδΉη­εθΌΈε₯ε p ε»ι€ιθ¨ n εΎηη΅ζγε ζ­€ζεε―δ»₯εΎε°δΈη΅θ―η«ζΉη¨εΌοΌθζ?η‘η΄ζηε½±ειεζΉζ³οΌζεζιθ¨θεΆδ»θ?ζΈεε₯η½?ζΌη­θε©ιοΌη?ζ¨ζε°ε nΒ² δΈ¦ζ½ε ζ­£εεΒ (Regularization)οΌεΎε° a θ b ε¨window k δΈ­ηζε°εΉ³ζΉεΌ
θεΆθ§£ηΊakθbk

εΆδΈ­οΌaβηεε­ηΊ I θ p ε°ζεΊ§ζ¨ windowηε±θ?η°ζΈΒ (covariance)οΌεζ―ηΊ I ε½±εδΈ­εεΊ§ζ¨ windows ηθ?η°ζΈγ

### Pseudo code
![](https://i.imgur.com/9yvErQm.png)

## Scene Radiance Recovery

In haze image, the atmospheric light is similar to the sky and we pick the top 0.1 percent brightest pixels in the depth map


π(π±)=(π(π±)βπ¨)/(min{maxβ‘(t(π±),0.1),0.9})+π¨

## Experimental Results

![](https://i.imgur.com/ROrPg3e.jpg)

![](https://i.imgur.com/wlDKQ7F.jpg)

![](https://i.imgur.com/QqmmvB1.jpg)

## References

[1] E. J. McCartney, Optics of the Atmosphere: Scattering by Molecules and Particles. New York, NY, USA: Wiley, 1976.

[2] Q. Zhu, J. Mai, and L. Shao, βA fast single image haze removal algorithm using color attenuation prior,β IEEE transactions on image processing, vol. 24, no. 11, pp. 3522β3533, 2015.

[3] K. He, J. Sun, and X. Tang, βGuided image filtering,β IEEE transactions on pattern analysis and machine intelligence, vol. 35, no. 6, pp. 1397β1409, 2012.

[4]ε°εζΏΎζ³’ηεηδ»₯εεΆζη¨. Guided Filter | by ι»η?θ | Medium
