/* 


original code : 
https://github.com/pkuwwt/LIC/blob/master/basic_lic.c
Chang Sha

gcc basic_lic.c -Wall -Wextra -lm
./a.out


*/



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strncat
#include <malloc.h>

// ---------------- global var  -----------------------

#define  SQUARE_FLOW_FIELD_SZ 400
#define  DISCRETE_FILTER_SIZE 2048
#define  LOWPASS_FILTR_LENGTH 10.00000f
#define  LINE_SQUARE_CLIP_MAX 100000.0f
#define  VECTOR_COMPONENT_MIN   0.050000f 




	int    n_xres = SQUARE_FLOW_FIELD_SZ;
	int    n_yres = SQUARE_FLOW_FIELD_SZ;
  
  	// dynamic float arrays ( fArrays )
	float*   pVectr; // Vector  field array
	// dynamic arrays for box filter LUTs 
	float*   p_LUT0; // = (float*   ) malloc( sizeof(float        ) * DISCRETE_FILTER_SIZE);
	float*   p_LUT1; // = (float*   ) malloc( sizeof(float        ) * DISCRETE_FILTER_SIZE);
  
  	// ucArrays : dynamic arrays for grayscale images ( unsigned char = 256 )
  	unsigned char* pNoise_ucArray; // = (unsigned char* ) malloc( sizeof(unsigned char) * n_xres * n_yres     );
	unsigned char* pLIC_ucArray; // = (unsigned char* ) malloc( sizeof(unsigned char) * n_xres * n_yres     );

	// https://www.sharetechnote.com/html/Calculus_VectorField.html
	// not all options work; not working : sink, source, square, spiral
	const char * sVectorFileldType[] = {"saddle" , "center", "source" , "sink", "other", "other2", "dipole", "sinus", "sinus2" , "sinus3", "simple", "square", "spiral", "cosinus", "bowl"};
	// enum items are contiguous 
	typedef enum  {saddle = 0 , center = 1, source = 2, sink = 3 , other = 4, other2 = 5, dipole = 6, sinus = 7, sinus2 = 8, sinus3 = 9, simple = 10, square = 11, spiral = 12, cosinus = 13, bowl = 14} VectorFileldType; 
	size_t VFmax; // length (size) of the array




// ------------------------- functions -------------------------------



///  compute VF type vector field and save it to pVectr ucArray 
void FillVextorField_ucArray(const VectorFileldType VF, const int  n_xres,  const int  n_yres,  float*  pVectr)
{
  int i,j; // indesxes of virtual 2D array
  float x,y;
  float t;
  
  for(j = 0;  j < n_yres;  j ++)
     for(i = 0;  i < n_xres;  i ++)
  	{ 
  		// 2D to 1D index conversion
   		int  index = (  (n_yres - 1 - j) * n_xres + i  )  <<  1; // index of pVectr 1D array
   		
   		// integer to float conversion of the coordinate = screen to world coordinate conversion
   		x =  (float) i / n_xres ;   
   		y =  (float) j / n_yres ; 
   		
   		// move origin to the center
   		x -= 0.5f; //  
   		y -= 0.5f; // 
   		
   		switch(VF) { // https://www.sharetechnote.com/html/Calculus_VectorField.html
    			case saddle:
				x =   x ; //  
				y = - y ; // 
				break;
				
			case center:
				x =  x ; //  
				y =  y ; // 
				break;

			case source:
				x =  x ; // 
				y =  y ; //
				break;
				
			case sink:
				x =  x ; // 
				y =  y ; // 
				break;
				
			// https://tex.stackexchange.com/questions/339243/plotting-streams-of-vector-fields	
			case other:
				t = 2.0*x*y; // temporary
				x =  x*x + t; // 
				y =  y*y + t; // 
				break;	
			
			// https://math.stackexchange.com/questions/536370/how-to-show-that-a-given-vector-field-is-not-complete-in-mathbbr2	
			case other2:
				x =  x*x ; // 
				y =  y*y ; // 
				break;	
			
			// https://github.com/anvaka/fieldplay/blob/master/Awesome%20Fields.md
			// True Dipole by /u/julesjacobs	
			case dipole:
				t = x*x; // temporary
				x =  2.0f*x*y ; // 
				y =  y*y - t; // 
				break;	
				
			
			
			//  v.x = sin(5.0*p.y + p.x);
			// v.y = cos(5.0*p.x - p.y);
			case sinus:
				t = x; // temporary
				x =  sin(10.0f*y + x); // 
				y =  cos(10.0f*t - y); // 
				break;	
				
			case sinus2:
				t = x; // temporary
				x =  sin(10.0f*x + y); // 
				y =  cos(10.0f*t + y); // 
				break;	
				
			case sinus3:
				t = x; // temporary
				x =  sin(10.0f*x + y); // 
				y =  cos(10.0f*y + t); // 
				break;	
				
			// https://github.com/anvaka/fieldplay
			case simple:
				x =  1.0f; // 
				y =  0.0f; // 
				break;	
				
			// v.x = -2.0 * mod(floor(y), 2.0) + 1.0;
			//v.y = -2.0 * mod(floor(x), 2.0) + 1.0;
			//https://github.com/anvaka/fieldplay
			case square:
				t = x;
				x =  -2.0f * (((int) floor(y)) % 2) + 1.0f; // 
				y =  -2.0f * (((int) floor(t)) % 2) + 1.0f; // 
				break;	
				
			// Dy=x, Dx=-y+.02x 
			case spiral:
				t = x;
				x = -y + 1.5f*x; // 
				y =  t; // 
				break;	
			//Kristen Beck https://www.geogebra.org/m/zYMbgTAN	
			case cosinus:
				t = x; // temporary
				x =  x; // 
				y =  cos(30.0f*y); // 
				break;	
			// https://www.intmath.com/blog/mathematics/vector-fields-a-simple-and-painless-introduction-3345
			// Vector fields - a simple and painless introduction By Murray Bourne	: A hilly bowl-shaped vector field
			case bowl:
				t = x; // temporary
				x =  1; // 
				y =  sin(50.f*t*t + 50.0f*y*y); // 
				break;	
			
	// https://demonstrations.wolfram.com/FlowOfAVectorFieldIn2D/
	//   Author:Juan Carlos Ponce Campuzano 
	//  x^2sin(y) ,   sqrt(y^2+x)exp(x/y) https://www.geogebra.org/m/cXgNb58T 
	// https://demonstrations.wolfram.com/VectorFieldsPlotExamples/
	// https://demonstrations.wolfram.com/VectorFieldsStreamlineThroughAPoint/
	// https://www.geogebra.org/m/kdw2vf9p		
	// https://www.reddit.com/r/fieldplay/
	// https://www.sharetechnote.com/html/Calculus_VectorField.html	
			default:
			}// switch
		
		// save values to the array 
		pVectr[index    ] =  y ;
   		pVectr[index + 1] =  x ;    
   		
   		
     	} 
}






///  normalize the vector field     ///
void    NormalizVectrs(const int  n_xres, const  int  n_yres,  float*  pVectr)
{
  int i,j;
  for(j = 0;  j < n_yres;  j ++)
     for(i = 0;  i < n_xres;  i ++)
     { 
   	int  index = (j * n_xres + i) << 1;
        float vcMag = (float)(  sqrt( (double)(pVectr[index] * pVectr[index] + pVectr[index + 1] * pVectr[index + 1]) )  );
   	float scale = (vcMag == 0.0f) ? 0.0f : 1.0f / vcMag;
   	pVectr[index    ] *= scale; // y
        pVectr[index + 1] *= scale; // x
     }
}




// Write_fArray2PPM



///  make white noise as the LIC input texture     ///
void FillNoise_ucArray(const int  n_xres, const  int  n_yres,  unsigned char*  pNoise_ucArray)
{
  int i,j;
  for( j = 0;   j < n_yres;  j ++)
     for( i = 0;   i < n_xres;  i ++)
     { 
   int  r = rand();
      r = (  (r & 0xff) + ( (r & 0xff00) >> 8 )  ) & 0xff;
         pNoise_ucArray[j * n_xres + i] = (unsigned char) r;
     }
}









///  generate box filter LUTs     ///
void    GenBoxFiltrLUT(const int  LUTsiz,  float*  p_LUT0,  float*  p_LUT1)
{   
     int i; 
     for(i = 0;  i < LUTsiz;  i ++)  p_LUT0[i] = p_LUT1[i] = i;
}






/*

const float*  p_LUT0 gives warning 
basic_lic.c:331:12: warning: assignment discards ‘const’ qualifier from pointer target type [-Wdiscarded-qualifiers]
  331 |     wgtLUT = (advDir == 0) ? p_LUT0 : p_LUT1;


*/
///  flow imaging (visualization) through Line Integral Convolution     ///
void Fill_LIC_array(const int n_xres, const int n_yres,  const float*  pVectr, const unsigned char*  pNoise_ucArray,  
        float*  p_LUT0,  float*  p_LUT1, const float   krnlen,  unsigned char*  pLIC_ucArray)
{ 
  int  vec_id;      ///ID in the VECtor buffer (for the input flow field)
  int  advDir;      ///ADVection DIRection (0: positive;  1: negative)
  int  advcts;      ///number of ADVeCTion stepS per direction (a step counter)
  int  ADVCTS = (int)(krnlen * 3); ///MAXIMUM number of advection steps per direction to break dead loops 
  
  float vctr_x;      ///x-component  of the VeCToR at the forefront point
  float vctr_y;      ///y-component  of the VeCToR at the forefront point
  float clp0_x;      ///x-coordinate of CLiP point 0 (current)
  float clp0_y;      ///y-coordinate of CLiP point 0 (current)
  float clp1_x;      ///x-coordinate of CLiP point 1 (next   )
  float clp1_y;      ///y-coordinate of CLiP point 1 (next   )
  float samp_x;      ///x-coordinate of the SAMPle in the current pixel
  float samp_y;      ///y-coordinate of the SAMPle in the current pixel
  float tmpLen;      ///TeMPorary LENgth of a trial clipped-segment
  float segLen;      ///SEGment   LENgth
  float curLen;      ///CURrent   LENgth of the streamline
  float prvLen;      ///PReVious  LENgth of the streamline  
  float W_ACUM;      ///ACcuMulated Weight from the seed to the current streamline forefront
  float texVal;      ///TEXture VALue
  float smpWgt;      ///WeiGhT of the current SaMPle
  float t_acum[2];     ///two ACcUMulated composite Textures for the two directions, perspectively
  float w_acum[2];     ///two ACcUMulated Weighting values   for the two directions, perspectively
  float* wgtLUT = NULL;    ///WeiGhT Look Up Table pointing to the target filter LUT
  float len2ID = (DISCRETE_FILTER_SIZE - 1) / krnlen; ///map a curve LENgth TO an ID in the LUT
  
  int i,j;
  
  ///for each pixel in the 2D output LIC image///
  for( j = 0;  j < n_yres;  j ++)
  for( i = 0;  i < n_xres;  i ++)
  { 
   	///init the composite texture accumulators and the weight accumulators///
   	t_acum[0] = t_acum[1] = w_acum[0] = w_acum[1] = 0.0f;
  
   	///for either advection direction///
         for(advDir = 0;  advDir < 2;  advDir ++)
         { 
    		///init the step counter, curve-length measurer, and streamline seed///
    		advcts = 0;
    		curLen = 0.0f;
             	clp0_x = i + 0.5f;
    		clp0_y = j + 0.5f;

    		///access the target filter LUT///
    		wgtLUT = (advDir == 0) ? p_LUT0 : p_LUT1;

    		///until the streamline is advected long enough or a tightly  spiralling center / focus is encountered///
             	while( curLen < krnlen && advcts < ADVCTS ) 
           	{
     			///access the vector at the sample///
     			vec_id = ( (int)(clp0_y) * n_xres + (int)(clp0_x) ) << 1;
     			vctr_x = pVectr[vec_id    ];
     			vctr_y = pVectr[vec_id + 1];

     			///in case of a critical point///
     			if( vctr_x == 0.0f && vctr_y == 0.0f )
     			{ 
      				t_acum[advDir] = (advcts == 0) ? 0.0f : t_acum[advDir];     ///this line is indeed unnecessary
      				w_acum[advDir] = (advcts == 0) ? 1.0f : w_acum[advDir];
      				break;
     			}
     
     			///negate the vector for the backward-advection case///
     			vctr_x = (advDir == 0) ? vctr_x : -vctr_x;
     			vctr_y = (advDir == 0) ? vctr_y : -vctr_y;

     			///clip the segment against the pixel boundaries --- find the shorter from the two clipped segments///
     			///replace  all  if-statements  whenever  possible  as  they  might  affect the computational speed///
     			segLen = LINE_SQUARE_CLIP_MAX;
     			segLen = (vctr_x < -VECTOR_COMPONENT_MIN) ? ( (int)(     clp0_x         ) - clp0_x ) / vctr_x : segLen;
     			segLen = (vctr_x >  VECTOR_COMPONENT_MIN) ? ( (int)( (int)(clp0_x) + 1.5f ) - clp0_x ) / vctr_x : segLen;
     			segLen = (vctr_y < -VECTOR_COMPONENT_MIN) ? 
        		(      (    (  tmpLen = ( (int)(     clp0_y)          - clp0_y ) / vctr_y  )  <  segLen    ) ? tmpLen : segLen      ) 
       				: segLen;
     					segLen = (vctr_y >  VECTOR_COMPONENT_MIN) ?
        				(      (    (  tmpLen = ( (int)( (int)(clp0_y) + 1.5f ) - clp0_y ) / vctr_y  )  <  segLen    ) ? tmpLen : segLen      ) 
       				: segLen;
     
     			///update the curve-length measurers///
     			prvLen = curLen;
     			curLen+= segLen;
     			segLen+= 0.0004f;
      
     			///check if the filter has reached either end///
     			segLen = (curLen > krnlen) ? ( (curLen = krnlen) - prvLen ) : segLen;

     			///obtain the next clip point///
     			clp1_x = clp0_x + vctr_x * segLen;
     			clp1_y = clp0_y + vctr_y * segLen;

     			///obtain the middle point of the segment as the texture-contributing sample///
     			samp_x = (clp0_x + clp1_x) * 0.5f;
     			samp_y = (clp0_y + clp1_y) * 0.5f;

     			///obtain the texture value of the sample///
     			texVal = pNoise_ucArray[( int)(samp_y) * n_xres + (int)(samp_x) ];

     			///update the accumulated weight and the accumulated composite texture (texture x weight)///
     			W_ACUM = wgtLUT[ (int)(curLen * len2ID) ];
     			smpWgt = W_ACUM - w_acum[advDir];   
     			w_acum[advDir]  = W_ACUM;        
     			t_acum[advDir] += texVal * smpWgt;
    
     			///update the step counter and the "current" clip point///
     			advcts ++;
     			clp0_x = clp1_x;
     			clp0_y = clp1_y;

     			///check if the streamline has gone beyond the flow field///
     			if( clp0_x < 0.0f || clp0_x >= n_xres || clp0_y < 0.0f || clp0_y >= n_yres)  break;
    		} // while( curLen < krnlen && advcts < ADVCTS ) 
          } // for(advDir = 0;  advDir < 2;  advDir ++)

   ///normalize the accumulated composite texture///
   texVal = (t_acum[0] + t_acum[1]) / (w_acum[0] + w_acum[1]);

   ///clamp the texture value against the displayable intensity range [0, 255]
   texVal = (texVal <   0.0f) ?   0.0f : texVal;
   texVal = (texVal > 255.0f) ? 255.0f : texVal; 
   pLIC_ucArray[j * n_xres + i] = (unsigned char) texVal;
  }
  
 
    
}



// Write_fArray2PPM ToDo




///  write the LIC image ( ucArray)  to a PPM file     ///
void Write_ucArray2PPM(const int  n_xres, const  int  n_yres, const char*  f_name, unsigned char*  pLIC_ucArray)
{ 
  FILE* o_file;
  if(   ( o_file = fopen(f_name, "w") )  ==  NULL   )  
  {  
   printf("Can't open output file\n");  
   return;  
  }

    fprintf(o_file, "P6\n%d %d\n255\n", n_xres, n_yres);
    int i,j;
    for( j = 0;  j < n_yres;  j ++)
     for( i = 0;  i < n_xres;  i ++)
  {
   	unsigned  char unchar = pLIC_ucArray[j * n_xres + i]; // read unchar value from ucArray
   	// save umchar value to the ppm file
        fprintf(o_file, "%c%c%c", unchar, unchar, unchar); // the same value for all rgb components = gray scale 
  }

    fclose (o_file); o_file = NULL;
}



void setup(){

	pVectr = (float*   ) malloc( sizeof(float        ) * n_xres * n_yres * 2 );
  	p_LUT0 = (float*   ) malloc( sizeof(float        ) * DISCRETE_FILTER_SIZE);
  	p_LUT1 = (float*   ) malloc( sizeof(float        ) * DISCRETE_FILTER_SIZE);
  
  
  	pNoise_ucArray = (unsigned char* ) malloc( sizeof(unsigned char) * n_xres * n_yres     );
  	pLIC_ucArray = (unsigned char* ) malloc( sizeof(unsigned char) * n_xres * n_yres     );
  	
  	VFmax =  sizeof(sVectorFileldType)/sizeof(sVectorFileldType[0]); // size of the array and Max Value of VF type
  	
  	// first input = noise
	FillNoise_ucArray(n_xres, n_yres, pNoise_ucArray);
  	Write_ucArray2PPM(n_xres, n_yres, "noise.ppm", pNoise_ucArray);
  	// filter
  	GenBoxFiltrLUT(DISCRETE_FILTER_SIZE, p_LUT0, p_LUT1);

}






void MakeLIC_Image(VectorFileldType VF){

	
  	
  	// compute second input = field
  	FillVextorField_ucArray(VF, n_xres, n_yres, pVectr);
  	NormalizVectrs(n_xres, n_yres, pVectr);
  	// save input type of vector field image - to do : Write_fArray2PPM
  	
  	
  	/* name of output graphic file */ 
	char name [120]; 
  	snprintf (name, sizeof name, "%s_LIC",  sVectorFileldType[VF]); // 
  	char *fileName =strcat(name,".ppm");
  	
  	// output = LIC image
  	Fill_LIC_array(n_xres, n_yres, pVectr, pNoise_ucArray, p_LUT0, p_LUT1, LOWPASS_FILTR_LENGTH, pLIC_ucArray);
  	Write_ucArray2PPM(n_xres, n_yres, fileName, pLIC_ucArray);

}


void end(){

	free(pVectr); 		pVectr = NULL;
	free(p_LUT0); 		p_LUT0 = NULL;
	free(p_LUT1); 		p_LUT1 = NULL;
	free(pNoise_ucArray); 	pNoise_ucArray = NULL;
	free(pLIC_ucArray); 	pLIC_ucArray = NULL;

}






int main(void)
{
  	setup();
	
	
	
	for(VectorFileldType VF = 0;  VF < VFmax;  ++ VF )
  		{MakeLIC_Image(VF);}

  	end();
  	
  	return 0;
}


