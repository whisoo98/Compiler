fprintf(listing, "Error: Undeclared function \"%s\" is called at line %d\n", name, lineno);//
fprintf(listing, "Error: Undeclared variable \"%s\" is used at line %d\n", name, lineno);//

fprintf(listing, "Error: Symbol \"%s\" is redefined at line %d\n",name , lineno ); //

fprintf(listing, "Error: Invalid array indexing at line %d (name : \"%s\"). Indices should be integer\n", lineno, name);//
fprintf(listing, "Error: Invalid array indexing at line %d (name : \"%s\"). Indexing can only be allowed for int[] variables\n", lineno, name);
fprintf(listing,  "Error: Invalid function call at line %d (name : \"%s\")\n", lineno, name);//

fprintf(listing, "Error: The void-type variable is declared at line %d (name : \"%s\")\n", lineno, name);//
 
fprintf(listing, "Error: Invalid operation at line %d\n", lineno);//
fprintf(listing, "Error: Invalid assignment at line %d\n", lineno);//
fprintf(listing, "Error: Invalid condition at line %d\n", lineno);//
fprintf(listing, "Error: Invalid return at line %d\n", lineno); //