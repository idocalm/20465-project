void appendAtStart(Node **head, void *data)
{
    /*
        Create a new node and swap the head with it. 
    */

    Node *newNode = (Node *) safe_malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = *head;
    *head = newNode;
}

void appendAtEnd(Node **head, void *data)
{
    /*
        Create a new node and append it to the end of the list.
    */

    Node *newNode = (Node *) safe_malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    Node *current = *head;
    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = newNode;
}

void freeList(Node *head)
{
    /*
        Free all the nodes in the list.
    */

    Node *current = head;
    Node *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }


}

