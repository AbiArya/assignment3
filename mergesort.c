


#include <stdlib.h>
#include <stdio.h>
#include "sorter_server.h"





Node* mergeSort(Node* head, int option){
	
	if(option == 0){
		return intMergeSort(head);
	}
	else if(option == 1){
		return floatMergeSort(head);

	}
	return stringMergeSort(head);
	
}
Node* floatMergeSort(Node* head){
	
	if(head == NULL || head->next == NULL) return head;
	
	Node* secondHalf = splitList(head);
	
	return mergeFloats(floatMergeSort(head), floatMergeSort(secondHalf));

}

Node* mergeInts(Node* head, Node* head2){


	if(head==NULL) return head2;
	if(head2==NULL) return head;
	
	Node* ptr = (Node*)malloc(sizeof(Node));
	Node* ret = ptr;

	while(head != NULL && head2 != NULL) {

		if(head2->number > head->number){
			ptr->next=head;
			head = head->next;
		}
		
		else {
			ptr->next = head2; 
			head2 = head2->next;
		}
		ptr = ptr->next;
		
	}

	if(head==NULL) ptr->next = head2;
	else{
		ptr->next = head;
	}    	

    	Node* reto = ret->next;
	free(ret);
    	return reto;

}

Node* intMergeSort(Node* head){
	
	if(head == NULL || head->next == NULL) return head;
	
	Node* secondHalf = splitList(head);
	
	return mergeInts(intMergeSort(head), intMergeSort(secondHalf));
	
}

Node* stringMergeSort(Node* head){
	
	if(head == NULL || head->next == NULL) return head;
	
	Node* secondHalf = splitList(head);
	
	return mergeStrings(stringMergeSort(head), stringMergeSort(secondHalf));

}

Node* mergeStrings(Node* head, Node* head2){

	
	if(head==NULL) return head2;
	if(head2==NULL) return head;
	
	Node* ptr = (Node*)malloc(sizeof(Node));
	ptr->next = NULL;
	Node* ret = ptr;
	
	while(head != NULL && head2 != NULL) {
		
		
		if(   strcmp(head2->word, head->word) > 0  ){
			
			ptr->next = head;
			head = head->next;
		}
		
		else {
			ptr->next = head2; 
			head2 = head2->next;
		}
		
		ptr = ptr->next;
		
	}
	
	if(head==NULL){
		ptr->next = head2;
	}

	else{
		ptr->next = head;
	}    	
	Node* reto = ret->next;
	free(ret);
    	return reto;


}


Node* mergeFloats(Node* head, Node* head2){


	if(head==NULL) return head2;
	if(head2==NULL) return head;
	
	Node* ptr = (Node*)malloc(sizeof(Node));
	Node* ret = ptr;

	while(head != NULL && head2 != NULL) {

		if(head2->dec > head->dec){
			ptr->next=head;
			head = head->next;
		}
		
		else {
			ptr->next = head2; 
			head2 = head2->next;
		}
		ptr = ptr->next;
		//printList(ptr);
	}

	if(head==NULL) ptr->next = head2;
	else{
		ptr->next = head;
	}    	


    	Node* reto = ret->next;
	free(ret);
    	return reto;



}

Node* splitList(Node* head){ //returns head of second half of list
	if(head==NULL || head->next==NULL) return NULL;
	Node* slow = head->next;
	Node* prev = head;
	Node* fast = head->next;

	while(fast!=NULL && fast->next!=NULL){

		slow = slow->next;
		prev = prev->next;	
		fast = fast->next->next;
		
		if(fast == NULL){
			break;
		}

	}
	prev->next = NULL;
	
	return slow;

}







