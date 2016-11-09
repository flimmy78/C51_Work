 #include <reg52.h>  
sbit p10 = P2^0;
sbit p11 = P2^1;
sbit p12 = P2^2;
sbit p13 = P2^3;

 void main()
 {
	 while(1)
	 {
	 	 /*dir2=1;
		 pwm2=0;
		 pwm2=1;

		 dir1=1;
		 pwm1=0;
		 pwm1=1;*/
		 		p11=1;p13=1;

				p10=0;
				p10=1;
				p12=1;
				p12=0;
	  }
 	
 }