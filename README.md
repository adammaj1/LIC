# LIC

Original repo : [Basic LIC by Chang Sha](https://github.com/pkuwwt/LIC/blob/master/basic_lic.c)

## Elements
* 2 inputs 
  * 2D vector field
  * white noise 
* one output ( LIC image)



__First input:__
* [strong wind](http://www.zhanpingliu.org/research/flowvis/LIC/LIC.htm) 
* a 2D static (=  time independent) saddle-shaped vector field  defined by a real-valued function F of two real variables x and y

  z = F(x,y)


Here is it's c implementation: 
```
void SyntheszSaddle(int  n_xres,  int  n_yres,  float*  pVectr)
{
  int i,j; 
  for(j = 0;  j < n_yres;  j ++) // y
     for(i = 0;  i < n_xres;  i ++) // x
  	{ 
   		int  index = (  (n_yres - 1 - j) * n_xres + i  )  <<  1;
   		pVectr[index    ] = - ( j / (n_yres - 1.0f) - 0.5f ); // y
   		pVectr[index + 1] =     i / (n_xres - 1.0f) - 0.5f;   // x 
     	} 
}

```

__Second input:__
* White noise  
* [massless fine sand](http://www.zhanpingliu.org/research/flowvis/LIC/LIC.htm)   

![](./png/noise.png "noise")  


__Output:__
* LIC image which ["emulates what happens when a rectangular area of massless fine sand is blown by strong wind"](http://www.zhanpingliu.org/research/flowvis/LIC/LIC.htm) )  
   
![](./png/LIC.png "LIC")  


## algorithm 


# See also
* [2DFlowVisualization by Andres Bejarano](https://github.com/andresbejarano/2DFlowVisualization)
* [fluid-simulator by Linus Mossberg](https://github.com/linusmossberg/fluid-simulator)
* [LIC by M. Harper Langston](https://github.com/harperlangston/LIC)
* [LIC by  Dzhelil Rufat](https://github.com/drufat/licpy)
* [LIC by greenhol](https://github.com/greenhol/deLICious)
* [fieldplay by anvaka](https://github.com/anvaka/fieldplay)
* [Vector-Field-Topolgy-2D by zaman13](https://github.com/zaman13/Vector-Field-Topolgy-2D)




# Git

create a new repository on the command line
```
echo "# LIC" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin git@github.com:adammaj1/LIC.git
git push -u origin main
```
## Local repo
```
~/Dokumenty/lic/basic_lic/ 

```




## Subdirectory

```git
mkdir png
git add *.png
git mv  *.png ./png
git commit -m "move"
git push -u origin main
```
then link the images:

```txt
![](./png/n.png "description") 

```


## Github
* [GitHub Flavored Markdown Spec](https://github.github.com/gfm/)
* [md cheat sheet](http://mdcheatsheet.com/)
* [CommonMark Spec](https://spec.commonmark.org)
* [Markdown parser ](https://markdown-it.github.io/)
