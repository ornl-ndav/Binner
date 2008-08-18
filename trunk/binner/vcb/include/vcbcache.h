#ifndef VCB_CACHE_H
#define VCB_CACHE_H

/** Struct
 * Name: vcbCacheKeyData
 * Version: 1.1
 * Description:
 * This struct contains the information describing a key in the Cookbook's
 * caching routines.  Since keys may be of any type, a simple <tt>void *</tt>
 * is used for generality.
 * Elements:
 *  void * key: This is a generic pointer to the starting byte of the key
 *             buffer.
 *  int len: This is the length in bytes of the key buffer starting at
 *           <tt>key</tt>.  Since no type is imposed on the key by any
 *           Cookbook functions, we do not know the length of the key without
 *           this value.
 **/

typedef struct {
   void *key;
   int len;
} vcbCacheKeyData;

/** Type
 * Name: vcbCache
 * Version: 1.1
 * Description:
 *  This type defines an instance of the Cookbook's caching data structure.
 *  Users can create a variable of this type and use it to increase performance
 *  of fetching of data from files, network, or computation by temporarily
 *  storing the processed or downloaded results.  Subsequent fetches will
 *  recall already processed data.  To create an instance of vcbCache, call
 *  vcbCacheMake.  See other ingredients in this header for more functions and
 *  data structures associated with the caching system.
 **/
typedef void *vcbCache;

/** Function
 * Name: vcbCacheFetch
 * Version: 1.6
 * Description:
 * This function fetches a list of arbitrary elements from (and possibly into)
 * an instance of a Cookbook-defined vcbCache.  If you are using a cache, you
 * should call this function even if the data hasn't yet been cached, as the
 * callback <tt>non_cache_read</tt> associated with the cache will be used to
 * retrieve and cache the missing data.
 *
 * <p>
 * The first element in the list is considered to be the only legitimately
 * requested element, while the rest are likely to be requested in the near
 * future and are only "prefetched."  These remaining may be neighboring
 * voxels, finer resolutions of a block, and so on.  These prefetched elements
 * may not be read into the cache if the cache is at maximum capacity and the
 * currently cached elements have a higher priority than the new.  (This is not
 * likely, however, unless lots of elements are prefetched at a time or the
 * cache is very small.)</p>
 *
 * <p>
 * If an element is not found in the cache, a function previously supplied
 * by the user is called to load the necessary data into the cache.  See
 * vcbCacheMake for more details.
 * Arguments:
 *  vcbCache cache: The cache (possibly) containing the elements to be
 *                  fetched.  This should have been returned from a call
 *                  to vcbCacheMake.
 *  vcbCacheKeyData ** keys:
 *                  The list of keys of elements to be fetched.  The first
 *                  element is guaranteed to be in the cache upon return, but
 *                  the remaining elements are prefetched and may not be in the
 *                  cache if they are not of a high enough priority.  When a
 *                  key from this list is no longer needed (because an element
 *                  and its key was already cached or when an element is
 *                  purged) the user-supplied free_key function is called.  
 *                  Otherwise, the memory associated with the key should remain
 *                  accessible and usable by the cache.
 *  int num_elements: The number of elements found in <tt>elements</tt>.
 * Return: void *; The data corresponding to the element first in the list is
 *                 returned.  This is a generic pointer that can be cast by
 *                 the user to an array of <tt>unsigned char</tt>s or any
 *                 application-specified type.  The returned data is whatever
 *                 was returned by the <tt>non_cache_read</tt> function
 *                 supplied to vcbCacheMake.
 * Known Issues: None
 **/
extern "C" void *vcbCacheFetch(vcbCache cache, vcbCacheKeyData **keys,
                               int num_elements);

/** Function
 * Name: vcbCacheReset
 * Version: 1.0
 * Description:
 *  This function clears an instance of vcbCache of all currently held
 *  elements, but retains the initialization parameters like capacity and user
 *  callbacks.  Cache statistics are also reset.
 * Arguments:
 *  vcbCache cache: The cache which should be reset.  This cache should have
 *                  have been created by calling vcbCacheMake.
 * Return: None
 * Known Issues: None
 **/
extern "C" void vcbCacheReset(vcbCache cache);

/** Function
 * Name: vcbCacheDestroy
 * Version: 1.0
 * Description:
 *  This function frees all resources used by an instance of vcbCache, 
 *  including the element-specific data for which the cache's user-supplied
 *  <tt>free_data</tt> function is used.
 * Arguments:
 *  vcbCache cache: The cache which should be freed.  This cache should have
 *                  have been created by calling vcbCacheMake.
 * Return: None
 * Known Issues: None
 **/
extern "C" void vcbCacheDestroy(vcbCache cache);

/** Function
 * Name: vcbCacheMake
 * Version: 1.2
 * Description:
 *  This function creates a cache for quick retrieval of data used repeatedly
 *  in the duration of an application.  Data that takes time to download or
 *  compute can be cached, and the next time that data is needed, instead of
 *  having to redownload or recompute, the previous results can be pulled from
 *  the cache.  
 * Arguments:
 *  void *(* non_cache_read) (vcbCacheKeyData *key_data, int *size): This is a 
 *  user-defined function that will pull data in that is requested with
 *  vcbCacheFetch but not yet in the cache.  Given a requested element's key,
 *  the function should compute, download, or process in whatever way the data
 *  for this element and return a pointer to that data's location in memory,
 *  cast to a <tt>void *</tt>.  When an element is in the cache, this memory
 *  address will be returned.  When elements are purged from the cache, this
 *  memory address is passed to the user-supplied <tt>free_data</tt> function.
 *  Additionally, the size of the data buffer should be stored in
 *  <tt>*size</tt>.  This value will be used to keep the cache under the
 *  size specified in vcbCacheMake.
 *  void (* free_data) (void *data): This is a user-defined function that
 *  releases the memory behind the <tt>void *</tt> returned by the user's
 *  <tt>non_cache_read</tt> function.  This function is called when a cache
 *  becomes overfull and an element is released, or when vcbCacheReset or
 *  vcbCacheDestroy are called.
 *  void (* free_key) (vcbCacheKeyData *key): This is a user-defined function
 *  that releases the memory behind a cache element's key (if necessary).  This
 *  function is called when a cache becomes overfull and an element is
 *  released, when vcbCacheReset or vcbCacheDestroy are called, or when a key
 *  is supplied to vcbCacheFetch and is not needed because an element and its
 *  key is already cached.
 *  void (* print_key) (vcbCacheKeyData *key_data):
 *   The user-defined function which takes an element's key and prints out some
 *   identifying tag.  If the key is a <tt>char *</tt> or <tt>int</tt>, for
 *   example, this function may just cast the key and display it with
 *   <tt>printf</tt>.  This function is only called from the vcbCacheStats
 *   routine, so if you do not ever call vcbCacheStats, this argument can be
 *   <tt>NULL</tt>.
 *  int maximum_capacity: This is the maximum number of bytes that
 *  the cache should hold.
 *  int num_purge: This is the minimum number of bytes to release from the
 *  cache when a request is made for an element not yet cached but when the
 *  cache is full.  This number must be at least 1, but it may be more.  Enough
 *  bytes to make room for the new element causing the cache miss will always
 *  be released, so <tt>num_purge</tt> is only meaningful when its value is
 *  greater than the size of the new element.  It allows the user to make more
 *  room than is instantaneously needed.
 *  int max_num_to_prefetch: This is the maximum number of elements that may
 *  queue up in the list of data to prefetch.  As new data are added to the
 *  prefetch list, data previously added to prefetch list that haven't yet been
 *  prefetched are removed from the list to prevent buildup of previous cache
 *  fetches.  Set this value to 0 if you do not want such trimming done.
 * Return: vcbCache; A handle to the cache is returned.  Any subsequent calls
 *  to cache functions must provide this handle.
 * Known Issues: None
 **/
extern "C" vcbCache vcbCacheMake(
                     void *(* non_cache_read) (vcbCacheKeyData *key_data, int *size),
                     void (* free_data) (void *data),
                     void (* free_key) (vcbCacheKeyData *key_data),
                     void (* print_key) (vcbCacheKeyData *key_data),
                     int maximum_capacity,
                     int num_purge,
                     int max_num_to_prefetch);

/** Function
 * Name: vcbCacheStats
 * Version: 1.2
 * Description:
 *  This function displays a summary of all elements contained in the cache
 *  and a short description of its performance: fullness and hit and miss
 *  rate.  To display the elements summary, a function to display an element's
 *  key is expected from the user.  A pointer to this function should be passed
 *  to vcbCacheMake.
 * Arguments:
 *  vcbCache cache: The cache whose statistics should be displayed.  This cache
 *                  should have have been created by calling vcbCacheMake.
 * Return: None
 * Known Issues: None
 **/
extern "C" void vcbCacheStats(vcbCache cache);

#endif

