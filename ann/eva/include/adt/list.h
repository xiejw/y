// copyright: see license_antirez-bsd file.

#ifndef EVA_LIST_H_
#define EVA_LIST_H_

// eva
#include <adt/value.h>

// -----------------------------------------------------------------------------
// A generic doubly linked list implementation
//
// node, list, and iterator are the only data structures used currently.
// -----------------------------------------------------------------------------

struct list_node {
        struct list_node *prev;
        struct list_node *next;
        union value       v;
};

struct list_iter {
        struct list_node *next;
        int               direction;  // 1 for toward forward, and 0 for tail
};

struct list {
        // internal fields.
        struct list_node *head;
        struct list_node *tail;
        void *( *dup )( void *ptr );             // optional, used by listDup
        void ( *free )( void *ptr );             // optional
        int ( *match )( void *ptr, void *key );  // optional
        unsigned long len;
};

// -----------------------------------------------------------------------------
// functions implemented as macros
// -----------------------------------------------------------------------------
#define listLen( l )      ( ( l )->len )
#define listHead( l )     ( ( l )->head )
#define listTail( l )     ( ( l )->tail )
#define listPrevNode( n ) ( ( n )->prev )
#define listNextNode( n ) ( ( n )->next )
#define listNodeData( n ) ( ( n )->v.data )

#define listSetDupFn( l, m )   ( ( l )->dup = ( m ) )
#define listSetFreeFn( l, m )  ( ( l )->free = ( m ) )
#define listSetMatchFn( l, m ) ( ( l )->match = ( m ) )

#define listGetDupFn( l )   ( ( l )->dup )
#define listGetFreeFn( l )  ( ( l )->free )
#define listGetMatchFn( l ) ( ( l )->match )

// -----------------------------------------------------------------------------
// prototypes
// -----------------------------------------------------------------------------
extern struct list *listNew( void );
extern void         listFree( struct list *list );
extern void         listClear( struct list *list );

// return new node or NULL if fails.
extern struct list_node *listAddNodeHead( struct list *list );
extern struct list_node *listAddNodeTail( struct list *list );
extern struct list_node *listInsertNode( struct list      *list,
                                         struct list_node *old_node,
                                         int               after );

// macro for common use.
#define listAppend( l, d ) ( listAddNodeTail( ( l ) )->v.data = ( d ) )

// del frees the node and (associated data inside).
// unlink just removes the data from the list.
extern void listDelNode( struct list *list, struct list_node *node );
extern void listUnlinkNode( struct list *list, struct list_node *node );

extern struct list      *listDup( struct list *orig );
extern struct list_node *listSearchKey( struct list *list, void *key );
extern struct list_node *listIndex( struct list *list, long index );

extern void listRotateTailToHead( struct list *list );
extern void listRotateHeadToTail( struct list *list );
extern void listJoin( struct list *l, struct list *o );  // move o into l

// -----------------------------------------------------------------------------
// iterator
// -----------------------------------------------------------------------------

extern struct list_iter *listNewIter( struct list *list, int direction );
extern void              listFreeIter( struct list_iter *iter );

extern struct list_node *listNext( struct list_iter *iter );

// Both rewind APIs reset all fields in 'li' to be ready of use.
//
// So the following code is recommended
//
//   struct list_iter li;  // place on stack.
//   listRewind(list, &li);
//
extern void listRewind( struct list *list, struct list_iter *li );
extern void listRewindTail( struct list *list, struct list_iter *li );

#endif  // EVA_LIST_H_
