/*#include "v8.h"

// Explicitly instantiate some template classes, so we're sure they will be
// present in the binary / shared object. There isn't much doubt that they will
// be, but MSVC tends to complain about these things.
#ifdef _MSC_VER
template class v8::Persistent<v8::Object>;
template class v8::Persistent<v8::FunctionTemplate>;
#endif*/
#include <assert.h>



	class ObjectWrap {
	public:
		ObjectWrap ( ) {
			refs_ = 0;
		}


		virtual ~ObjectWrap ( ) {
			if (!handle_.IsEmpty()) {
				assert(handle_.IsNearDeath());
				handle_.ClearWeak();
				handle_->SetInternalField(0, v8::Undefined());
				handle_.Dispose();
				handle_.Clear();
			}
		}


		template <class T>
		static inline T* Unwrap (v8::Handle<v8::Object> handle) {
			assert(!handle.IsEmpty());
			assert(handle->InternalFieldCount() > 0);
			return static_cast<T*>(handle->GetPointerFromInternalField(0));
		}


		v8::Persistent<v8::Object> handle_; // ro

		inline void Wrap (v8::Handle<v8::Object> handle) {
			assert(handle_.IsEmpty());
			assert(handle->InternalFieldCount() > 0);
			handle_ = v8::Persistent<v8::Object>::New(handle);
			handle_->SetPointerInInternalField(0, this);
			MakeWeak();
		}

	protected:

		inline void MakeWeak (void) {
			handle_.MakeWeak(this, WeakCallback);
			handle_.MarkIndependent();
		}

		/* Ref() marks the object as being attached to an event loop.
		* Refed objects will not be garbage collected, even if
		* all references are lost.
		*/
		virtual void Ref() {
			assert(!handle_.IsEmpty());
			refs_++;
			handle_.ClearWeak();
		}

		/* Unref() marks an object as detached from the event loop.  This is its
		* default state.  When an object with a "weak" reference changes from
		* attached to detached state it will be freed. Be careful not to access
		* the object after making this call as it might be gone!
		* (A "weak reference" means an object that only has a
		* persistant handle.)
		*
		* DO NOT CALL THIS FROM DESTRUCTOR
		*/
		virtual void Unref() {
			assert(!handle_.IsEmpty());
			assert(!handle_.IsWeak());
			assert(refs_ > 0);
			if (--refs_ == 0) { MakeWeak(); }
		}


		int refs_; // ro


	private:
		static void WeakCallback (v8::Persistent<v8::Value> value, void *data) {
			ObjectWrap *obj = static_cast<ObjectWrap*>(data);
			assert(value == obj->handle_);
			assert(!obj->refs_);
			assert(value.IsNearDeath());
			delete obj;
		}
	};