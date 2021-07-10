import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';
import { HttpClient} from '@angular/common/http';
import { map } from 'rxjs/operators';
import { GetScriptsResponse, Script } from './script';
import { environment } from '../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class ScriptService {

  constructor(private http: HttpClient) { }

  private baseUrl = environment.baseURL + 'api/scripts';

  getScripts(): Observable<Script[]> {
    const jobs = this.http.get<GetScriptsResponse>(this.baseUrl).
      pipe(map(response => response.response.map(data => {
        return data;
      })));
    return jobs;
  }

  getScriptContent(script: Script): Observable<string>
  {
    const content = this.http.get(this.baseUrl + '/' + script.name, { responseType: 'text' });
    return content;
  }

  /** POST: run new test job */
  updateScript(script: Script, content: string): void {
    const req = this.http.post(this.baseUrl + '/' + script.name, content);
    req.subscribe();
  }

  cloneRepo(): void
  {
    const req = this.http.put(environment.baseURL + 'api/repo/clone', '');
    req.subscribe();
  }

  /**
   * Handle Http operation that failed.
   * Let the app continue.
   * @param operation - name of the operation that failed
   * @param result - optional value to return as the observable result
   */
  private handleError<T>(operation = 'operation', result?: T) {
    return (error: any): Observable<T> => {

      // TODO: send the error to remote logging infrastructure
      console.error(operation, error); // log to console instead

      // Let the app keep running by returning an empty result.
      return of(result as T);
    };
  }
}
