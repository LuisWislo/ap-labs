//Luis Wilson A00226649
int mystrlen(char *str){
    char c;
    int i=0;
    while(c = str[i] != '\0'){
       i++; 
    }
    return i;
}

char *mystradd(char *origin, char *addition){

    char *final = origin;

    while(*origin != '\0'){
        origin++;
    }

    while(*addition != '\0'){
        *origin++ = *addition++;
    }

    *origin = '\0';

    return final;
}

int mystrfind(char *origin, char *substr){

    int i=0, j=0,k;
    int length = mystrlen(origin);
    int subLength = mystrlen(substr);

    if(subLength == 0)
        return 1;
    if(length == 0)   
        return 0;
    if(subLength > length)
        return 0;
    
    while(i < length){
        if(origin[i++] == substr[0]){
            j=i;
            k=1;
            int canReturn = 1;
            
            while(k < subLength){
                if(j<length){
                    if(origin[j++] != substr[k++]){
                        canReturn = 0;
                        break;        
                    }
                } else {
                    return 0;
                }
            }
            if(canReturn)
                return 1;
        }
    }
    return 0;
}
