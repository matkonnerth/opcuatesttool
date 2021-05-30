import { Injectable, NgZone } from '@angular/core';
import { Observable, of, Observer } from 'rxjs';
import { HttpClient} from '@angular/common/http';
import { catchError, map } from 'rxjs/operators';
import { environment } from '../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class ReplService {

  constructor(private http: HttpClient, private _zone: NgZone) { }

  private baseUrl = environment.baseURL + 'api/repl';

  /** POST: new input to lua interpreter */
  newLine(content: string): void {
    const req = this.http.post(this.baseUrl, content);
    req.subscribe();
  }

  getServerSentEvent(): Observable<string> {
    return new Observable((observer: Observer<string>) => {
      const eventSource = this.getEventSource(environment.baseURL + 'api/events');
      eventSource.onmessage = event => {
        this._zone.run(() => {
          observer.next(event.data);
        });
      };
      eventSource.onerror = error => {
        this._zone.run(() => {
          observer.error(error);
        });
      };
    });
  }

  getEventSource(uri: string): EventSource
  {
    return new EventSource(uri);
  }
}
