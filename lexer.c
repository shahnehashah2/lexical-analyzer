#include<stdio.h>

int line=1;
void dfa();

typedef  int (*pfuncptr)(char c,FILE *op);
typedef pfuncptr (*pptrfuncptr)(char c,FILE *op);

pfuncptr d1(char c,FILE *op);
pfuncptr d2(char c,FILE *op);
pfuncptr d3(char c,FILE *op);
pfuncptr d4(char c,FILE *op);
pfuncptr d5(char c,FILE *op);
pfuncptr d6(char c,FILE *op);

pfuncptr trapcom(char c,FILE *op);

pfuncptr startcom(char c,FILE *op)
{
	if(c=='/')
        {
               	return (pfuncptr)d1;	//If it is the start of comments goto d1
         }
        if(c=='#')
        {
	       	return (pfuncptr)d2;	//If it is a preprocessor directive goto d2
         }
        else
              	return (pfuncptr)startcom;

}


pfuncptr d1(char c,FILE *op)
{
	if(c=='/')
        {
    		return (pfuncptr)d3;
        }

        if(c=='*')
	{
        	return (pfuncptr)d4;	//If it is a multiline comment goto d4
	}
        else
        	{
			putc('/',op);
        		return (pfuncptr)startcom;
                }
}
pfuncptr d3(char c,FILE *op)
{
	if(c!='\n')
        {
                return (pfuncptr)d3;
        }
        if(c=='\n')
        {
        	putc('\n',op);
	        return (pfuncptr)trapcom; 		//End of single line comment
        }
}



pfuncptr d4(char c,FILE *op)
{
	if(c=='*')
        {
                                 /* '*' can be just another char in comment or it might be signalling the end of 					            comments.Check this condition in d5*/
                return (pfuncptr)d5;
        }
        if(c=='\n')
        {
        	putc('\n',op);
        }

	        return (pfuncptr)d4;          //else keep reading the other characters inside the comment
}


pfuncptr d5(char c,FILE *op)
{
         if(c=='/')             //If previous char is '*' and next char is '/' then '*/' indicate end of comm
         return (pfuncptr)trapcom;    //then goto trap state.
         return (pfuncptr)d4;		//otherwise '*' is just another char in comment.goto d4 and continue reading 						//the comments until end of comment
}


pfuncptr d2(char c,FILE *op)
{
        if(c=='\n')
        {
        	putc('\n',op);
        	return (pfuncptr)trapcom;
        }

	else
        {
		return (pfuncptr)d2;
        }
}


pfuncptr trapcom(char c,FILE *op)
{
       	return (pfuncptr)trapcom;
}



void preproc(FILE *f)
{
        char ch;
	int i;
        FILE *op=fopen("preproc.tmp","w");

        if(!op)
        {
        	printf("\nPreproc:Error opening file..");
                exit(1);
        }
        pptrfuncptr state=startcom;
        while( (ch=fgetc(f)) != EOF)
	{
        	state=(pptrfuncptr)(*state)(ch,op);

                if(state==trapcom)
                {
                    state=startcom;
                }

                else if(state==startcom)
                {
                    putc(ch,op);
                }

        }
        fclose(op);

	return;
 } 

main(int argc,char *argv[])
{
  FILE *f;
  int i=0,j;
  char str[20],c; 

  f=fopen(argv[1],"r");
  if(!f)
    {
      printf("\nError in opening the input file\n");
      exit(1);
    }
  preproc(f);
  fclose(f);
  dfa();
}

int is_ident(const char *str)
  {
    int i=0;
    if(str[i]=='_'||(str[i]>='a'&& str[i]<='z')||(str[i]>='A'&&
    str[i]<='Z'))
      {
       i++;
       while(str[i]!='\0')
         {
          if(str[i]=='_'||(str[i]>='a'&&str[i]<='z')||
            (str[i]>='A'&&str[i]<='Z') ||(str[i]>='0'&&str[i]<='9'))
              i++;
          else  
            {printf(" < ERR_IDEN >");
              break;
             }
         }
       if(str[i]=='\0')
         {
              printf( "  < %s,identifier>",str);
         }
       else    
         {
              return 0;
         }
     }
   else
     return 0;
}

int is_keyword(const char* str)
{
  static char *arr[63]={
  "asm","auto","bool","break","case","catch","char","class","const",
  "const_cast","continue","default","delete","do","double","dynamic_cast",
  "else","enum","explicit","export","extern","false","float","for",
  "friend","goto","if","inline","int","long","mutable","namespace","new",
   "operator","private","protected","public","register","reinterpret_cast",
  "return","short","signed","sizeof","static","static_cast","struct",
   "switch","template","this","throw","true","try","typedef","typeid",
  "typename","union","unsigned","using","virtual","void","volatile",
   "wchar_t","while"};
  int upper=62,lower=0;
  int mid;
  int found=0;
  //start the binary search...
  while(upper>=lower && !found)
    {
      mid=(upper+lower)/2;
      if(strcmp(str,arr[mid])==0)
        found=1;
      if(strcmp(str,arr[mid])<0)
        upper=mid-1;
      else
        lower=mid+1;
    }

  if(!found)
    return 0;
  switch(mid)
    {
    case 1:
       case 37:
    case 43:
    case 20:
    case 52:printf("  < %s,STOR_SPEC>",str);
            return 1;
    case 6:
    case 59:
    case 40:
    case 28:
    case 22:
    case 29:
    case 24:
    case 14:
    case 41:
    case 56:
    case 45:
    case 55:
    case 17:printf("  < %s,TYPE_SPEC>",str);
            return 1;
    case 8:
    case 60:printf("  < %s,TYPE_QUAL>",str);
            return 1;
    case 4:
    case 11:printf("  < %s,LABEL_KEY>",str);
            return 1;
    case 26:
    case 16:
    case 46:printf("  < %s,SEL_KEY>",str);
            return 1;
    case 62:
    case 13:
    case 23:printf("  < %s,ITER_KEY>",str);
            return 1;
    case 25:
    case 10:
    case 3:
    case 39:printf("  < %s,JMP_KEY>",str);
            return 1;
        case 42:printf("  < %s,keyword>",str);
            return 1;
    default:printf("  < %s,c++ keyword>",str);
            return 1;
    }
}



int is_octal(const char*str)
{
    int i=0;
    if(str[i]=='0' && str[i+1]!='x')
     {
      i++;
      while(str[i]!='u' && str[i]!='U' && str[i]!='l' &&
            str[i]!='L' && str[i]!='\0')
        {
        if(str[i]>='0'&& str[i]<='7')
                i++;
         else
                {printf("  < ERR_IN..oct >");break;}
        }
      if(str[i]=='u'||str[i]=='U')
        {
            i++;
            if(str[i]=='l'||str[i]=='L')
              {
                i++;
                if(str[i]=='\0')
                  {
                   printf("  < %s,unsigned long octal>",str);
                   return 1;
                  }
                else
                  {
                  printf(" < ERR_IN..oct >");
                  return 0;
                  }
              }
            else
                if(str[i]=='\0')
                  {
                   printf("  < %s,unsigned octal>",str);         
                   return 1;
                  }
                else
                  {
                   printf(" < ERR_IN..oct >");
                   return 0;
                  } 

         }
      else
           if(str[i]=='l'||str[i]=='L')
             {
              i++;
              if(str[i]=='u'||str[i]=='U')
                { 
                 i++;
                 if(str[i]=='\0')
                   {
                    printf("  < %s,unsigned long octal>",str);      
                    return 1;
                   }
                 else
                   {
                    printf(" < ERR_IN..oct >");
                     return 0;
                   }
                }
                   
              else
                {
                 if(str[i]=='\0')
                     {
                      printf("  < %s,long octal>",str);
                      return 1;
                     }
                 else
                    {
                      printf(" < ERR_IN..oct >");
                      return 0;
                    }
                 } 
              
             }
           else
             if(str[i]=='\0')
                {
                  printf("  < %s,octal>",str);
                     return 1;
                }
             else 
                     return 0;
   }
else
    return 0;
}

int is_dec(const char *str)
  { 
   int i=0;
   if(str[i]>='1' && str[i]<='9')
     {
     i++;
     while(str[i]!='u' && str[i]!='U'&& str[i]!='l' && str[i]!='L' && 
           str[i]!='\0')
            {
               if(str[i]>='0' && str[i]<='9')
                   i++;
               else
                  {/*printf("  < ERR_IN >");*/break;}
             }
        if(str[i]=='u'||str[i]=='U')
            {
              i++; 
              if(str[i]=='l'||str[i]=='L')
                {
                 i++;
                   if(str[i]=='\0')
                      {
                         printf("  < %s,unsigned long decimal>",str);
                         return 1;
                       }
                    else
                      { printf(" < ERR_IN..dec >");return 0;}
                 }
               else
                 if(str[i]=='\0')
                   {
                     printf("  < %s,unsigned decimal>",str);
                     return 1;
                   }
                else
                  { printf(" < ERR_IN..dec >");return 0;}
 
                 
             }
        else 
              if(str[i]=='l'||str[i]=='L')
                {
                 i++;
                 if(str[i]=='u'||str[i]=='U')
                   {  
                      i++;
                       if(str[i]=='\0')
                       {    
                        printf("  < %s,unsigned long decimal>",str);
                        return 1;
                        }
                        else
                        {printf(" < ERR_IN..dec >");return 0;}
                   }
                  else
                     if(str[i]=='\0')
                       {
                        printf("  < %s,long decimal>",str);
                        return 1;
                        }
                  else
                     { printf(" < ERR_IN..dec >");return 0;}    
                 }
               else
                  if(str[i]=='\0')
                   {
                     printf("  < %s,decimal>",str);
                     return 1;
                   }
                  else
                    {printf(" < ERR_IN..dec >");return 0;}
         }
      else
           return 0;
}
 
int is_hexa(const char* str)
{
   int i=0;
   if((str[i]=='0')&&(str[i+1]=='x'||str[i+1]=='X'))
     {
      i=i+2;
      while(str[i]!='u' && str[i]!='U' && str[i]!='l'&& str[i]!='L'
              && str[i]!='\0')
         {
            if((str[i]>='0' && str[i]<='9')||(str[i]>='a'&& str[i]<='f')
                ||(str[i]>='A' && str[i]<='F'))                        
                 i++;
             else
                {printf("  < ERR_IN..hex >");break;}
         }
       if(str[i]=='u'||str[i]=='U')
         {
           i++;
           if(str[i]=='l'||str[i]=='L')
             {
               i++;
               if(str[i]=='\0')
                {
                 printf("  < %s,unsigned long hexadecimal>",str);
                 return 1;
                }
                else  
                  return 0;
              }
            else 
              if(str[i]=='\0')
                {
                  printf("  < %s,unsigned hexadecimal>",str);
                  return 1;
                 }
            }
        else
            if(str[i]=='l'||str[i]=='L')
              {
                i++;
               if(str[i]=='u'||str[i]=='U')
                 {
                    i++;
                    if(str[i]=='\0')
                   {
                     printf("  < %s,unsigned long hexadecimal>",str);
                     return 1;
                   }
                    else
                      return 0;
                 }
                else
                   if(str[i]=='\0')
                      {
                       printf("  < %s,long hexadecimal>",str);
                       return 1;
                       }
                }
            else 
             if(str[i]=='\0')
                {
                   printf("  < %s,hexadecimal>",str);
                   return 1;
                 }
              else
                 return 0;
    }
 else
    return 0;
}  

/*
int is_float(const char* str)
 {
  int i=0;
   while((str[i]!='.')&&(str[i]!='\0'))
   {
       if(str[i]>='0' && str[i]<='9')
         i++;
        else
          break;
    }
    if(str[i]=='.')
     {
         i++;
      while((str[i]!='f') && (str[i]!='F')&&(str[i]!='l')&&(str[i]!='L')&&
      (str[i]!='e')&&(str[i]!='E')&&(str[i]!='\0'))
         {
           if(str[i]>='0' && str[i]<='9')
                 i++;
           else
               break;
          }
            if((str[i]=='e')||(str[i]=='E'))
          {
           i++;
           if(str[i]=='\0')
              return 0;
  while((str[i]!='f')&&(str[i]!='F')&&(str[i]!='l')&&(str[i]!='L')
          &&(str[i]!='\0'))
           {
            if((str[i]>='0')&&(str[i]<='9'))
               i++;
            else
               {printf("  < Float error>");break;}
            }
  if(((str[i]=='f')||(str[i]=='F')||(str[i]=='l')||(str[i]=='L'))
        &&(str[i+1]=='\0'))
            {
        printf("  < %s,floating point number>",str);
        return 1;
            }
       else
            if(str[i]=='\0')
              {
              printf("  < %s,floating point number>",str);
              return 1;
             }
            else
               return 0;

        }
  else
      if(((str[i]=='f')||(str[i]=='F')||(str[i]=='l')||(str[i]=='L'))&&
           (str[i+1]=='\0'))
           {
                printf("  < %s,floating point number>",str);
                return 1;
            }
      else
           if(str[i]=='\0')
              {
               printf("  < %s,floating point number>",str);
               return 1;
              }
            else
               return 0;
     }
 else
     return 0;
}
*/      

int is_float(const char* str)
 {
  int i=0;
     
      while((str[i]!='e')&&(str[i]!='E')&&(str[i]!='\0'))
         {
           if(str[i]>='0' && str[i]<='9')
                 i++;
           else 
               break;
          }
      if((str[i]=='e')||(str[i]=='E'))
          {
           i++;
           if(str[i]=='\0')
              return 0;
           while((str[i]!='f')&&(str[i]!='F')&&(str[i]!='l')&&(str[i]!='L')
                 &&(str[i]!='\0'))
               {
               if((str[i]>='0')&&(str[i]<='9'))
                   i++;
               else
                   {printf("  < ERR_FL >");break;}
                   }
               if(((str[i]=='f')||(str[i]=='F')||(str[i]=='l')||(str[i]=='L'))
                  &&(str[i+1]=='\0'))
                   {
                    printf("  < %s,floating point number>",str);
                    return 1;
                    }
               else
                    if(str[i]=='\0')
                       {
                       printf("  < %s,floating point number>",str);
                        return 1;   
                         }
                     else
                        return 0;
     
                }
}

void dfa()
{
 int i=0,j,k,l,chk,len=0,line1;
 char c,a,word[256],word1[2][256],string[256],character[256];
 FILE *fp=fopen("preproc.tmp","r");
 printf("\n1");
 back3:while((c=fgetc(fp)) != EOF)
      {
       if(c=='\n')
	   {
            line++;
            printf("\n\n%d",line);
           }
       back:if(isalnum(c)||c=='_')
                word[i++]=c;
	    else
                if(i!=0)
                  {
                    word[i]='\0';
                    chk=is_keyword(word);
                    if(chk==0)
                       chk=is_ident(word);
                    if(chk==0)
                       chk=is_hexa(word);
                    if(chk==0)
                        chk=is_octal(word);
                     if(chk==0)
                        chk=is_float(word);
                     if(chk==0)
                        chk=is_dec(word);
	          i=0;}
 
 back2:
       if(c=='.')
	 {
	 if((c=fgetc(fp))=='.')
	    {
	    if((c=fgetc(fp))=='.')
		{
		printf("  < ...,ELLIPSE>");
		goto back3;
		}
	    else
		{
		printf("  < ..,error>");
		goto back;
		}
	    }
	 else
	     {
	     printf("  < .,DOT_OP>");
	     goto back;
	     }
	  }

       if(c=='+')
         {
           if((c=fgetc(fp))=='+')                                 
             printf("  < ++,UN_OP>");
           else
		if(c=='=')
		    {
		    printf("  < +=,ASS_OP>");
   		    goto back3;
		    }
		else
		    {
		    printf("  < +,ADD_OP>");
		    goto back;
		    }
	}
      if(c=='-')
	  {
	  if((c=fgetc(fp))=='-')
		printf("  < --,UN_OP>");
	   else
		if(c=='=')
		   {
		   printf("  < -=,ASS_OP>");
		   goto back3;
		   }
		else
		   {
		   if(c=='>')
			{
			printf("  < ->,DEREF_OP>");
			goto back3;
			}
		   else
			{
			printf("  < -,ADD_OP>");
			goto back;
			}
 		}
	   }
	if(c=='*')
	   {
	   if((c=fgetc(fp))=='=')
		{
		printf("  < *=,ASS_OP>");
		goto back3;
		}
	   else
		{
		printf("  < *,MUL_OP>");
		goto back;
		}
	   }
	if(c=='/')
	   {
	   if((c=fgetc(fp))=='=')
		{
		printf("  < /=,ASS_OP>");
		goto back3;
		}
	   else
		{
		printf("  < /,MUL_OP>");
		goto back;
		}
	   }
	if(c=='&')
	   {
	   if((c=fgetc(fp))=='&')
		{
		printf("  < &&,AND_OP>");
		goto back3;
		}
	   else
		{
		if(c=='=')
		    {
		    printf("  < &=,ASS_OP>");
		    goto back3;
		    }
		else
		    {
		    printf("  < &,IN_AND>");
		    goto back;
		    }
                }    
            }

	
	if(c=='=')
	    {
	    if((c=fgetc(fp))=='=')
		{
		printf("  < ==,EQ_OP>");
		goto back3;
		}
	    else
		{
		printf("  < =,ASS_OP>");
		goto back;
		}
	    }
	if(c=='|')
	    {
	    if((c=fgetc(fp))=='|')
		{
		printf("  < ||,OR_OP>");
		goto back3;
		}
	    else
		{
		if(c=='=')
		    {
		    printf("  < |=,ASS_OP>");
		    goto back3;
		    }
		else
	 	    {
		    printf("  < |,IN_OR>");
		    goto back;
		    }
	        }
            }
	if(c=='!')
	    {
	    if((c=fgetc(fp))=='=')
		{
		printf("  < !=,EQ_OP>");
		goto back3;
		}
	    else if(c=='<' || c=='>')
		{
		printf("  < !%c,REL_OP>",c);
		goto back3;
		}
	    else
		{
		printf("  < !,UN_OP>");
		goto back;
		}
	    }
	if(c=='<')
	    {
	    if((c=fgetc(fp))=='=')
		{
		printf("  < <=,ASS_OP>");
		goto back3;
		}
	    else
		{
		if(c=='<')
		    {
		    if((c=fgetc(fp))=='=')
			{
			printf("  < <<=,REL_OP>");
			goto back3;
			}
		    else
			{
			printf("  < <<,SH_OP>");
			goto back;
			}
		    }
		else
		    {
		    printf("  < <,REL_OP>");
		    goto back;
		    }
	        }

	    }	    

        if(c=='>')
            {
            if((c=fgetc(fp))=='=')
                {
                printf("  < >=,ASS_OP>");
                goto back3;
                }
            else
                {
                if(c=='>')
                    {
                    if((c=fgetc(fp))=='=')
                        {
                        printf("  < >>=,REL_OP>");
                        goto back3;
                        }
                    else
                        {
                        printf("  < >>,SH_OP>");
                        goto back;
                        }
                    }
                else
                    {
                    printf("  < >,REL_OP>");
                    goto back;
                    }
                }
            }
        if(c=='%')
	    {
	    if((c=fgetc(fp))=='=')
		{
		printf("  < %=,ASS_OP>");
		goto back3;
		}
	    else
		{
		printf("  < %,MUL_OP>");
		goto back;
		}
   	    }
	
        if(c=='~')
            {
            if((c=fgetc(fp))=='=')
                {
                printf("  < ~=,ASS_OP>");
                goto back3;
                }
            else
                {
                printf("  < ~,UN_OP>");
                goto back;
                }
            }
	
	if(c==':')
	    {
	    if((c=fgetc(fp))==':')
		{
		printf("  < ::,SCOPE_OP>");
		goto back3;
		}
  	    }
        
        if(i==0 && (c==':' || c==',' ||c=='?' || c=='(' || c==')' || c=='}'
               || c=='{' || c=='[' || c==']' || c==';'))
           printf(" %c",c);
 	if(c=='\'')
	    {
	    character[0]='\'';
	    back4:
		while((c=fgetc(fp))!='\'')
		    {
		    if(c=='\n')line++;
		    character[++len]=c;
		    }
 	    if(character[len]=='\\')			/*for an input like '\''*/
		{character[++len]=c;
		if(len>2)
		    {
	  	    printf("  < ERR_CH >");
		    goto back;
		    }
	        goto back4;}
	    character[++len]=c;
	    character[++len]='\0';
	    if((character[1]=='\\' && character[3]=='\'')
		||(character[2]=='\''))
	        printf("  < %s,CHAR_C>",character);
	    else
		printf("  < ERR_CH >");
	    len=0;
	    }

        if(c=='"')
            {
            len=0;
            string[len]='"';
            back5:
                while((c=fgetc(fp))!='"')
                    {if(c==EOF)
			{
			printf("  < ERR_EOF>");
			exit(1);
			}
                    if(c=='\n')line++;
                    string[++len]=c;
                    }
	    if(string[len]=='\\')                  
                {string[++len]=c;
                goto back5;}
	        string[++len]='"';
                string[++len]='\0';
	        printf("  < %s,STR_C>",string);
            }
        }
    }
		    
	    
		    	














