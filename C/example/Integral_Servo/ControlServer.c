#include <unistd.h>
#include <stdio.h>
#include "Msequence.h"
#include "Quantizer.h"

float control(float Vo, float V1,float Vr){
	static float Ve = 0.0;
	Ve += (Vo-Vr)*100e-3;
	/* pole place @ 0.75+0.1i, 0.75-0.1i, -0.02 */
	float Vi = -1.2460*Vo - 0.1081*V1 - 1.6422*Ve;
	if(Vi>2.5){
		Vi=2.5;
	}else if(Vi<-2.5){
		Vi=-2.5;
	}

	return Vi;
}

void server(int sockfd){
	char buf[8]={};
	char write_buf[8]={};
	float Vo,V1,Vr,Vi;
	FILE *fp;

    if(read(sockfd,buf,2) < 0){
        close(sockfd);
        return ;
    }
	Vr = 4.0 * (mseq() - 0.5);
	Vo=Uni_dequantizer(buf[0]);
	V1=Uni_dequantizer(buf[1]);
	Vi=control(Vo,V1,Vr);

	printf("%f,%f,%f,%f\n",Vr,Vo,V1,Vi);

	write_buf[0]=Uni_quantizer(Vi);
    if(write(sockfd,write_buf,1) < 0){
        close(sockfd);
        return ;
    }

	if((fp=fopen("data.csv","a")) != NULL){
		fprintf(fp,"%f,%f,%f,%f\n",Vr,Vo,V1,Vi);
		fclose(fp);
	}

	close(sockfd);
}

